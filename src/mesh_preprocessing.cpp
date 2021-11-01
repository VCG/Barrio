#include "mesh_preprocessing.h"


typedef CGAL::Simple_cartesian<double>              k;
using point = k::Point_3;
typedef k::Triangle_3                               triangle;
typedef std::list<triangle>::iterator               iterator;
typedef CGAL::AABB_triangle_primitive<k, iterator>  primitive;
typedef CGAL::AABB_traits<k, primitive>             aabb_triangle_traits;
typedef CGAL::AABB_tree<aabb_triangle_traits>       tree;

MeshProcessing::MeshProcessing()
{
	qDebug() << "Init Mesh Preprocessor";
}

MeshProcessing::~MeshProcessing()
{
	qDebug() << "Destroy Mesh Preprocessor";
}

struct MyStructure {
	std::string name;
	std::list<triangle> triangles;
	std::vector<point> vertices;
};

int MeshProcessing::computeCenter(Object* obj, std::vector<VertexData>* vertices)
{
	struct MyStructure my_structure;
	my_structure.name = obj->getName();

	std::vector<int>* indices = obj->get_indices_list();

	float volume = 0.0;
	float surface_area = 0.0;

	// extract vertex data
	for (auto i = 0; i < indices->size(); i = i + 3)
	{
		// vertex 0
		const int idx_0 = (*indices)[i]; // dereferencing pointer
		auto v0 = vertices->at(idx_0);
		point p0(v0.x(), v0.y(), v0.z());
		my_structure.vertices.push_back(p0);

		// vertex 1
		const int idx_1 = (*indices)[i + 1]; // dereferencing pointer
		auto v1 = vertices->at(idx_1);
		point p1(v1.x(), v1.y(), v1.z());
		my_structure.vertices.push_back(p1);

		// vertex 2
		const int idx_2 = (*indices)[i + 2]; // dereferencing pointer
		auto v2 = vertices->at(idx_2);
		point p2(v2.x(), v2.y(), v2.z());
		my_structure.vertices.push_back(p2);

		// add face
		my_structure.triangles.emplace_back(p0, p1, p2);

		float v321 = p2.x() * p1.y() * p0.z();
		float v231 = p1.x() * p2.y() * p0.z();
		float v312 = p2.x() * p0.y() * p1.z();
		float v132 = p0.x() * p2.y() * p1.z();
		float v213 = p1.x() * p0.y() * p2.z();
		float v123 = p0.x() * p1.y() * p2.z();

		volume += (1.0f / 6.0f) * (-v321 + v231 + v312 - v132 - v213 + v123);

		QVector3D a = QVector3D(p0.x(), p0.y(), p0.z());
		QVector3D b = QVector3D(p1.x(), p1.y(), p1.z());
		QVector3D c = QVector3D(p2.x(), p2.y(), p2.z());

		surface_area += computeSurfaceArea(a, b, c);
	}

	point c = CGAL::centroid(my_structure.triangles.begin(), my_structure.triangles.end(), CGAL::Dimension_tag<2>());
	QVector4D center(c.x(), c.y(), c.z(), 1.0);
	obj->setCenter(center);
	obj->setVolume(abs(volume));
	obj->setSurfaceArea(surface_area);

	return EXIT_SUCCESS;
}



// Load astrocyte mesh and build its tree
// Load other objects (neurites)
// for each other object, check against the astrocyte, and get the closest vertices from neurites to skeleton mesh
// Goal: find all closest points to astrocyte

int MeshProcessing::compute_distance_distribution(Object* mito, Object* cell, std::vector<VertexData>* vertices)
{
	struct MyStructure my_cell;
	my_cell.name = cell->getName();

	std::vector<int>* cell_indices = cell->get_indices_list();

	// extract vertex data
	for (auto i = 0; i < cell_indices->size(); i = i + 3)
	{
		// vertex 0
		const int idx_0 = (*cell_indices)[i]; // dereferencing pointer
		auto v0 = vertices->at(idx_0);
		point p0(v0.x(), v0.y(), v0.z());

		// vertex 1
		const int idx_1 = (*cell_indices)[i + 1]; // dereferencing pointer
		auto v1 = vertices->at(idx_1);
		point p1(v1.x(), v1.y(), v1.z());

		// vertex 2
		const int idx_2 = (*cell_indices)[i + 2]; // dereferencing pointer
		auto v2 = vertices->at(idx_2);
		point p2(v2.x(), v2.y(), v2.z());

		if (isBorderVertex(v0.x(), v0.y(), v0.z()) || isBorderVertex(v1.x(), v1.y(), v1.z()) || isBorderVertex(v2.x(), v2.y(), v2.z()))
		{
			continue;
		}

		my_cell.vertices.push_back(p0);
		my_cell.vertices.push_back(p1);
		my_cell.vertices.push_back(p2);

		// add face
		my_cell.triangles.emplace_back(p0, p1, p2);
	}

	// load astrocyte into a tree
	//qDebug() << QString(my_cell.name.c_str()) << " has: " << my_cell.triangles.size() << " Triangles";

	//qDebug() << "Started building distance tree ...";
	tree tree(my_cell.triangles.begin(), my_cell.triangles.end());
	tree.accelerate_distance_queries();
	//qDebug() << "Finished building distance tree ...";

	const auto mito_indices = mito->get_indices_list();

	double max_distance = 0.0;

	// compute distances for each mitochondrion vertex
	for (int idx : *mito_indices)
	{
		auto vertex = &vertices->at(idx);

		if (!isBorderVertex(vertex->x(), vertex->y(), vertex->z()))
		{
			point point_query(vertex->x(), vertex->y(), vertex->z());
			const auto distance = std::sqrt(tree.squared_distance(point_query));
			if (distance > max_distance)
				max_distance = distance;
			vertex->distance_to_cell = distance;
		}
		else
		{
			vertex->distance_to_cell = std::numeric_limits<float>::max();
		}

	}

	qDebug() << "Done computing distance distribution between " << QString(mito->getName().c_str()) << " and " << QString(cell->getName().c_str());

	return EXIT_SUCCESS;
}

double MeshProcessing::compute_closest_distance(Object* from, Object* to, std::vector<VertexData>* vertices)
{
	struct MyStructure structure;
	structure.name = to->getName();

	std::vector<int>* cell_indices = to->get_indices_list();

	// extract vertex data
	for (auto i = 0; i < cell_indices->size(); i = i + 3)
	{
		// vertex 0
		const int idx_0 = (*cell_indices)[i]; // dereferencing pointer
		auto v0 = vertices->at(idx_0);
		point p0(v0.x(), v0.y(), v0.z());

		// vertex 1
		const int idx_1 = (*cell_indices)[i + 1]; // dereferencing pointer
		auto v1 = vertices->at(idx_1);
		point p1(v1.x(), v1.y(), v1.z());

		// vertex 2
		const int idx_2 = (*cell_indices)[i + 2]; // dereferencing pointer
		auto v2 = vertices->at(idx_2);
		point p2(v2.x(), v2.y(), v2.z());

		structure.vertices.push_back(p0);
		structure.vertices.push_back(p1);
		structure.vertices.push_back(p2);

		// add face
		structure.triangles.emplace_back(p0, p1, p2);
	}

	// load astrocyte into a tree
	//qDebug() << QString(structure.name.c_str()) << " has: " << structure.triangles.size() << " Triangles";

	//qDebug() << "Started building distance tree ...";
	tree tree(structure.triangles.begin(), structure.triangles.end());
	tree.accelerate_distance_queries();
	//qDebug() << "Finished building distance tree ...";

	const auto from_indices = from->get_indices_list();

	double min_distance = std::numeric_limits<double>::max();

	// compute distances for each mitochondrion vertex
	for (int idx : *from_indices)
	{
		// dereferencing pointer
		auto vertex = &vertices->at(idx);
		point point_query(vertex->x(), vertex->y(), vertex->z());
		const auto distance = std::sqrt(tree.squared_distance(point_query));
		if (distance < min_distance)
			min_distance = distance;
	}

	qDebug() << "Closest Distance between " << QString(from->getName().c_str()) << " to " << QString(to->getName().c_str()) << ": " << min_distance;

	return min_distance;
}

float MeshProcessing::computeSurfaceArea(QVector3D a, QVector3D b, QVector3D c)
{
	QVector3D ca = a - c;
	QVector3D cb = b - c;

	QVector3D crossProd = QVector3D::crossProduct(ca, cb);

	return 0.5 * crossProd.length();
}

bool MeshProcessing::isBorderVertex(float x, float y, float z)
{
	float delta = 0.06;

	bool onXBorder = x >= MESH_MAX_X - delta || x <= delta;
	bool onYBorder = y >= MESH_MAX_Y - delta || y <= delta;
	bool onZBorder = z >= MESH_MAX_Z - delta || z <= delta;

	if (onXBorder || onYBorder || onZBorder)
	{
		return true;
	}

	return false;
}
