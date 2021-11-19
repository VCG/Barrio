#include <chrono>
#include "datacontainer.h"
#include <set>
#include <QDomDocument>

#include "mesh_preprocessing.h"

/*
 * m_objects -> object class for each object (astrocyte, dendrite, ..)
 *           -> get from this the indices of the mesh
 *           -> get the skeleton vertices
 */
DataContainer::DataContainer(InputForm* input_form) :
	m_indices_size(0),
	m_skeleton_points_size(0),
	max_volume(2),
	max_astro_coverage(0),
	max_synapse_volume(0),
	m_vertex_offset(0),
	m_faces_offset(0)
{
	m_mesh_processing = new MeshProcessing();
	m_mesh = new Mesh();
	objs_filename = QString("/objects.cereal");

	loadData();
}

DataContainer::~DataContainer()
{
	qDebug() << "~Mesh()";
	for (std::size_t i = 0; i != m_objects.size(); i++) {
		delete m_objects[i];
	}
}

void DataContainer::loadData()
{
	QString data_path(PATH + QString("data"));
	QString cache_subpath = QString("/cache_m3");

	QString neurites_path = data_path + "/m3_data/m3_all_corrected.obj";
	QString semantic_skeleton_path = data_path + "/m3_data/skeletons.json";
	//QString neurite_skeletons_path = data_path + "/m3_data/m3_neurite_skeletons.json";

	if (true)
	{
		auto t1 = std::chrono::high_resolution_clock::now();
		importObj(neurites_path);
		auto t2 = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double, std::milli> ms = t2 - t1;
		qDebug() << "Objects Loading time: " << ms.count();
		qDebug() << "faces: " << m_mesh->getFacesListSize();
		qDebug() << "vertices: " << m_mesh->getVerticesSize();
		qDebug() << "normals: " << m_mesh->getNormalsListSize();

		compute_centers();
		compute_distance_mito_cell_boundary();
		//compute_closest_distance_to_structures();

		qDebug() << "------------ Writing data to cache now ---------------";
		writeDataToCache(data_path + cache_subpath);
		qDebug() << "------------ Done writing data to cache ---------------";
	}
	else
	{
		loadDataFromCache(data_path + cache_subpath);	
	}

	importSemanticSkeleton(semantic_skeleton_path);
	//importSkeletons(neurite_skeletons_path, Object_t::DENDRITE);
	//importSkeletons(mitos_skeletons_path, Object_t::MITO);
}

char* DataContainer::loadRawFile(QString path, int size)
{
	QFile  file(path);
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "Could not open the file for reading";
		return false;
	}

	QDataStream in(&file);
	in.setVersion(5);
	char* buffer = new char[size];
	in.device()->reset();
	int nbytes = in.readRawData(buffer, size);

	return  buffer;
}

bool DataContainer::isWithinDistance(int from, int to, float threshold)
{
	Object* from_object = getObject(from);
	std::map<int, double>* distances = from_object->get_distance_map_ptr();

	if (distances->count(to))
	{
		if (distances->at(to) <= threshold)
		{
			return true;
		}
	}
	return false;
}

bool DataContainer::hasSemanticSkeleton(int hvgx)
{
	QJsonDocument doc = QJsonDocument::fromJson(m_sematic_skeleton_json.toUtf8());
	std::map<int, Object*>* objects = getObjectsMapPtr();

	QJsonArray output;

	for (auto it : doc.array()) {
		QJsonObject element = it.toObject();
		int id = element["id"].toInt();

		if (hvgx == id)
		{
			return true;
		}
	}
	return false;
}

void DataContainer::dumpObjects(QString path)
{
	std::map<int, Object> objs;
	for (auto const& [hvgxID, obj] : m_objects)
	{
		objs[hvgxID] = *obj;
	}

	std::ofstream ofs(path.toStdString(), std::ios::binary); // any stream can be used
	{
		cereal::BinaryOutputArchive oarchive(ofs); // Create an output archive  
		oarchive(objs); // Write the data to the archive
	} // archive goes out of scope, ensuring all contents are flushed
}

void DataContainer::readObjects(QString path)
{
	std::ifstream ifs(path.toStdString(), std::ios::binary);
	{
		cereal::BinaryInputArchive iarchive(ifs); // Create an input archive
		iarchive(serializable_objects); // Read the data from the archive
	}

	// fill m_objects map
	for (auto& [hvgxID, obj] : serializable_objects)
	{
		obj.getSkeleton()->setID(obj.getHVGXID());
		m_objectsByType[obj.getObjectType()].push_back(&obj);

		if (m_indices_size_byType.find(obj.getObjectType()) == m_indices_size_byType.end()) {
			m_indices_size_byType[obj.getObjectType()] = 0;
		}
		m_indices_size_byType[obj.getObjectType()] += obj.get_indices_size();
		m_indices_size += obj.get_indices_size();

		m_objects[hvgxID] = &obj;
	}

}

int DataContainer::getObjectThatHasSynapse(int syn_id, int not_this_object)
{
	std::vector<Object*> dends = getObjectsByType(Object_t::DENDRITE);
	for (auto dendrite : dends)
	{
		int dend_hvgx = dendrite->getHVGXID();
		if (dend_hvgx != not_this_object && dendrite->hasSynapse(syn_id))
		{
			return dend_hvgx;
		}
	}

	std::vector<Object*> axons = getObjectsByType(Object_t::AXON);
	for (auto axon : axons)
	{
		int axon_hvgx = axon->getHVGXID();
		if (axon_hvgx != not_this_object && axon->hasSynapse(syn_id))
		{
			return axon_hvgx;
		}
	}
	return -1;
}

QList<int> DataContainer::getMitosOfCell(int cell_id)
{
	QList<int> mitos;
	Object* cell = getObjectsMap().at(cell_id);
	std::vector<int>* children = cell->getChildrenIDs();

	for (int i : *children) {
		bool isMito = getObjectsMap().at(i)->getObjectType() == Object_t::MITO;
		if (isMito) {
			mitos.append(i);
		}
	}
	return mitos;
}

bool DataContainer::importObj(QString path)
{
	int vertexCounter = 0;
	int normalCounter = 0;

	QFile inputFile(path);
	if (!inputFile.open(QIODevice::ReadOnly))
	{
		qDebug() << "Cant open " << path;
	}

	int hvgxID = 0;
	Object* obj = NULL;
	std::vector< struct VertexData >* meshVertexList = m_mesh->getVerticesList();

	QTextStream in(&inputFile);
	while (!in.atEnd()) {

		QString line = in.readLine();
		QList<QString> elements = line.split(" ");

		// parse object names
		if (!strcmp(elements[0].toStdString().c_str(), "o"))
		{
			QList<QString> nameList = elements[1].split('_');
			hvgxID = nameList.last().toInt();

			QString name; // name consits of everything but the last entry of namelist
			for (int i = 0; i < nameList.size() - 1; ++i)
				name += nameList[i] + "_";
			name.remove(name.length() - 1, name.length() - 1); // remove last character

			qDebug() << "Reading " << name;

			obj = new Object(name.toUtf8().constData(), hvgxID);

			obj->setVolume(2.0f);
			obj->setAstPoint(QVector4D(1.0f / MESH_MAX_X, 1.0f / MESH_MAX_Y, 1.0f / MESH_MAX_Z, 0));

			m_objects[hvgxID] = obj;
			m_objectsByType[obj->getObjectType()].push_back(obj);
		}

		// parse vertices
		else if (!strcmp(elements[0].toStdString().c_str(), "v")) { // read vertices

			float x = abs(elements[1].toFloat());
			float y = abs(elements[2].toFloat());
			float z = abs(elements[3].toFloat());

			vertexCounter++;

			QVector4D mesh_vertex(x, y, z, float(hvgxID));

			meshVertexList->emplace_back();
			int vertexIdx = (int)meshVertexList->size() - 1;
			struct VertexData* v = &meshVertexList->at(vertexIdx);

			v->vertex = mesh_vertex;
			v->distance_to_cell = 0.0; // first init all distances with the default value
			v->hvgxID = hvgxID;
			v->structure_type = (int)obj->getObjectType();
			v->is_skeleton = false;
		}

		// parse faces
		else if (!strcmp(elements[0].toStdString().c_str(), "f")) // read triangulated faces
		{
			int vertexIndex[3];

			vertexIndex[0] = elements[1].toInt();
			vertexIndex[1] = elements[2].toInt();
			vertexIndex[2] = elements[3].toInt();

			if (!m_mesh->isValidFaces(vertexIndex[0], vertexIndex[1], vertexIndex[2])) {
				qDebug() << "Error in obj file! - invalid faces";
				return false;
			}

			// safe indices counter clockwise
			obj->addTriangleIndex(vertexIndex[0] - 1);
			obj->addTriangleIndex(vertexIndex[2] - 1);
			obj->addTriangleIndex(vertexIndex[1] - 1);

			// safe indices counter clockwise
			m_mesh->addFace(vertexIndex[0] - 1, vertexIndex[2] - 1, vertexIndex[1] - 1);
			m_indices_size += 3;

			if (m_indices_size_byType.find(obj->getObjectType()) == m_indices_size_byType.end()) {
				m_indices_size_byType[obj->getObjectType()] = 0;
			}
			m_indices_size_byType[obj->getObjectType()] += 3;
		}
	}
	inputFile.close();

	addSliceVertices();

	m_mesh->computeNormalsPerVertex();
	processParentChildStructure();

	qDebug() << "Done reading .obj file";
	qDebug() << vertexCounter << " vertices";
	qDebug() << normalCounter << " normals";

	return true;
}

bool DataContainer::importSemanticSkeleton(QString path)
{
	QString json = QString();
	QFile inputFile(path);
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			json.append(in.readLine());
		}
		inputFile.close();
	}
	m_sematic_skeleton_json = json;

	return true;
}

MySkeleton DataContainer::processSkeletonStructure(int hvgx, QList<int>* indices, std::vector<struct VertexData>* vertices)
{
	MySkeleton skeleton;
	skeleton.hvgx = hvgx;

	QVector<bool> visited(QVector<bool>(indices->size()));

	// compute fork vertices
	QVector<int> fork_vertices;
	for (int i = 0; i < indices->length(); i++)
	{
		int idx = indices->at(i);
		int count = indices->count(idx);
		if ((count > 2 || count == 1) && !fork_vertices.contains(idx))
		{
			qDebug() << i << " --> " << count;
			fork_vertices.push_back(idx);
		}
	}

	int start;
	int branch_counter = 0;
	while (visited.count(true) < visited.size())
	{

		// find starting point
		for (int i = 0; i < visited.size(); i = i + 2)
		{
			MyEdge edge_candidate;
			edge_candidate.p1 = indices->at(i);
			edge_candidate.p2 = indices->at(i + 1);

			if (!visited[i] && edgeTouchesForkVertex(edge_candidate, &fork_vertices))
			{
				start = i;
				break;
			}
		}

		MyBranch branch;

		int p1 = indices->at(start);

		branch.start_index = p1;

		// compute branch
		double radius = 0.0; // in nn
		double length = 0.0; // in nn

		bool terminate = false;
		double vertex_count = 0;
		double min_radius = DBL_MAX;
		double max_radius = 0;

		while (!terminate)
		{
			// p1 and p2 form an edge
			MyEdge edge;
			edge.p1 = indices->at(start);
			edge.p2 = indices->at(start + 1);

			if (!branch.indices.contains(edge.p1))
			{
				branch.indices.push_back(edge.p1);
			}
			if (!branch.indices.contains(edge.p2))
			{
				branch.indices.push_back(edge.p2);
			}

			branch.end_index = edge.p2;

			visited[start] = true;
			visited[start + 1] = true;

			VertexData v1 = vertices->at(edge.p1);
			VertexData v2 = vertices->at(edge.p2);

			if (v1.distance_to_cell < min_radius) {
				min_radius = v1.distance_to_cell;
			}
			if (v2.distance_to_cell < min_radius) {
				min_radius = v2.distance_to_cell;
			}

			if (v1.distance_to_cell > max_radius) {
				max_radius = v1.distance_to_cell;
			}
			if (v2.distance_to_cell > max_radius) {
				max_radius = v2.distance_to_cell;
			}

			radius += v1.distance_to_cell;
			radius += v2.distance_to_cell;

			QVector3D d = v2.vertex.toVector3D() + v1.vertex.toVector3D();
			length += abs(d.length());

			vertex_count += 1.0;

			if (vertex_count > 1.5 && edgeTouchesForkVertex(edge, &fork_vertices)) // this part is done
			{
				terminate = true;
				break;
			}
			else
			{
				int i = 0;
				while (i < indices->size())
				{
					if ((!visited.at(i) && edgeContainsIndex(edge, indices->at(i))) ||
						(!visited.at(i + 1) && edgeContainsIndex(edge, indices->at(i + 1))))
					{
						start = i;
						break;
					}
					i = i + 2;
					if (i == indices->size()) // reached the end of the list
					{
						terminate = true;
						break;
					}
				}
			}
		}

		double avg_radius = radius / vertex_count;

		branch.avg_radius = avg_radius;
		branch.length = length;
		branch.vertex_count = vertex_count;
		branch.min_radius = min_radius;
		branch.max_radius = max_radius;

		qDebug() << "min radius: " << min_radius;
		qDebug() << "max radius: " << max_radius;

		skeleton.branches[branch_counter] = branch;
		branch_counter++;
	}

	for (int key : skeleton.branches.keys())
	{
		MyBranch branch = skeleton.branches.value(key);
		for (int i : branch.indices) {
			VertexData* v = &vertices->at(i);
			v->vertex.setW(key);
		}
	}
	return skeleton;
}

bool DataContainer::importSkeletons(QString neurite_skeleton_path, Object_t type)
{
	QFile file;
	file.setFileName(neurite_skeleton_path);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	QByteArray data = file.readAll();
	file.close();
	QJsonDocument skeletons_doc = QJsonDocument::fromJson(data);

	QJsonObject object = skeletons_doc.object();

	std::vector< struct VertexData >* meshVertexList = m_mesh->getVerticesList();

	int index_offset = meshVertexList->size();

	foreach(const QString & key, object.keys())
	{
		QJsonValue skeleton = object.value(key);

		int hvgx = 0;
		if (type == Object_t::MITO)
		{
			hvgx = 1000 + key.toInt();
		}
		else
		{
			hvgx = key.toInt() + 1;
		}

		QJsonDocument skel_doc = QJsonDocument::fromJson(skeleton.toString().toUtf8());

		QJsonArray edges = skel_doc.object().value("edges").toArray();
		QJsonArray vertices = skel_doc.object().value("vertices").toArray();
		QJsonArray radii = skel_doc.object().value("radius").toArray();

		// process per vertex data
		for (int i = 0; i < vertices.size(); i++)
		{
			QJsonArray v = vertices.at(i).toArray();

			double z = v.at(0).toDouble() / (DIM_X + 1);
			double x = v.at(1).toDouble() / (DIM_Y + 1);
			double y = v.at(2).toDouble() / (DIM_X + 1);

			double radius = radii.at(i).toDouble();

			QVector4D mesh_vertex(x, y, z, 1.0);

			meshVertexList->emplace_back();
			int vertexIdx = (int)meshVertexList->size() - 1;
			struct VertexData* vertex = &meshVertexList->at(vertexIdx);

			vertex->vertex = mesh_vertex;
			vertex->distance_to_cell = radius;
			vertex->hvgxID = hvgx;
			vertex->structure_type = getTypeByID(hvgx);
			vertex->is_skeleton = true;
		}

		// process indices
		QList<int> intermediate_edges;
		for (int i = 0; i < edges.size(); i++)
		{
			QJsonArray e = edges.at(i).toArray();

			int p1 = e.at(0).toInt() + index_offset;
			int p2 = e.at(1).toInt() + index_offset;

			intermediate_edges.push_back(p1);
			intermediate_edges.push_back(p2);

			m_skeleton_indices.push_back(p1);
			m_skeleton_indices.push_back(p2);
		}

		index_offset = meshVertexList->size();
	}

	return true;
}

bool DataContainer::edgeTouchesForkVertex(MyEdge edge, QVector<int>* fork_vertices)
{
	if (fork_vertices->contains(edge.p1) || fork_vertices->contains(edge.p2))
	{
		return true;
	}
	return false;
}

bool DataContainer::edgeContainsIndex(MyEdge edge, int index)
{
	if (edge.p1 == index || edge.p2 == index)
	{
		return true;
	}
	return false;
}

void DataContainer::display(const std::vector<MyBranch>* branches, int n)
{
	for (MyBranch b : *branches) {
		std::cout << " " << b.avg_radius;
	}
	std::cout << std::endl;
}

void DataContainer::findPermutations(QMap<int, MyBranch>* b)
{
	std::vector<int> branches = b->keys().toVector().toStdVector();

	branches.begin();

	do {
		for (int id : branches) {
			std::cout << " " << id;
		}
		std::cout << std::endl;
	} while (std::next_permutation(branches.begin(), branches.end()));
}

void DataContainer::processParentChildStructure()
{
	std::vector<Object*> mitos = getObjectsByType(Object_t::MITO);
	for (auto const& mito : mitos)
	{
		QString name = mito->getName().c_str();
		QList<QString> nameList = name.split("_");

		if (name.contains("A")) // mito of axon
		{
			name = nameList[1].replace("A", "Axon");
		}
		else if (name.contains("D")) // mito of dendrite
		{
			name = nameList[1].replace("D", "Dendrite");
		}
		else if (name.contains("Astro")) // mito of astrocyte
		{
			// todo
		}

		Object* parent = getObjectByName(name);
		mito->setParentID(parent->getHVGXID());
		parent->addChildID(mito->getHVGXID());
	}

	std::vector<Object*> spines = getObjectsByType(Object_t::SPINE);
	for (auto const& spine : spines)
	{
		QString name = spine->getName().c_str();
		QList<QString> nameList = name.split("_");
		int parentID = nameList[1].toInt();

		Object* parent = m_objects.at(parentID);
		spine->setParentID(parent->getHVGXID());
		parent->addChildID(spine->getHVGXID());
	}

	std::vector<Object*> boutons = getObjectsByType(Object_t::BOUTON);
	for (auto const& bouton : boutons)
	{
		QString name = bouton->getName().c_str();
		QList<QString> nameList = name.split("_");
		int parentID = nameList[1].toInt();

		Object* parent = m_objects.at(parentID);
		bouton->setParentID(parent->getHVGXID());
		parent->addChildID(bouton->getHVGXID());
	}

	std::vector<Object*> synapses = getObjectsByType(Object_t::SYNAPSE);
	for (auto const& syn : synapses)
	{
		QString name = syn->getName().c_str();
		QList<QString> nameList = name.split("_");
		QString synapse_identifier = nameList[1];

		// parse dendrite name
		QString dendrite_identifier = synapse_identifier.mid(0, 4);
		QString dendrite_name = dendrite_identifier.replace("D", "Dendrite");
		Object* dendrite = getObjectByName(dendrite_name);
		if (dendrite != nullptr)
		{
			dendrite->addSynapse(syn);
			syn->setParentID(dendrite->getHVGXID());
		}

		// parse axon name
		QString axon_identifier = synapse_identifier.mid(7, 4);
		qDebug() << "Axon identifier: " << axon_identifier;
		QString axon_name = axon_identifier.replace("A", "Axon");
		Object* axon = getObjectByName(axon_name);
		if (axon != nullptr)
		{
			axon->addSynapse(syn);
			syn->setParentID(axon->getHVGXID());
		}
	}
}

int DataContainer::getTypeByID(int hvgx)
{
	if (m_objects.count(hvgx) > 0)
	{
		Object* obj = m_objects.at(hvgx);
		return (int)obj->getObjectType();
	}

	return -1;
}

Mesh* DataContainer::getMesh()
{
	return m_mesh;
}

int DataContainer::getMeshIndicesSize()
{
	return m_indices_size;
}

std::map<int, Object*>  DataContainer::getObjectsMap()
{
	return m_objects;
}

int DataContainer::getIndexByName(QString name)
{
	for (auto const& [id, obj] : m_objects)
	{
		QString map_name = obj->getName().c_str();
		if (QString::compare(map_name, name) == 0)
		{
			return id;
		}
	}

	return -1;
}

void DataContainer::writeDataToCache(QString cache_path)
{
	m_mesh->dumpMesh(cache_path);
	dumpObjects(cache_path + objs_filename);
}

void DataContainer::loadDataFromCache(QString cache_path)
{
	m_mesh->readMesh(cache_path);
	readObjects(cache_path + objs_filename);
}

void DataContainer::compute_distance_mito_cell_boundary()
{
	std::vector<Object*> mitos = m_objectsByType[Object_t::MITO];

	for (int i = 0; i < mitos.size(); i++)
	{
		Object* mito = mitos[i];
		Object* parent = m_objects[mito->getParentID()];
		m_mesh_processing->compute_distance_distribution(mito, parent, m_mesh->getVerticesList());
	}
}

void DataContainer::compute_centers()
{
	for (auto const& [hvgxID, obj] : m_objects)
	{
		m_mesh_processing->computeCenter(obj, m_mesh->getVerticesList());
	}
}

void DataContainer::compute_synapse_distances(Object* mito)
{
	std::vector<Object*> synapses = m_objectsByType.at(Object_t::SYNAPSE);
	for (Object* syn : synapses)
	{
		double closest_distance = m_mesh_processing->compute_closest_distance(mito, syn, m_mesh->getVerticesList());
		mito->setDistanceToStructure(syn->getHVGXID(), closest_distance);
	}
}

void DataContainer::compute_mito_distances(Object* synapse)
{
	std::vector<Object*> mitos = m_objectsByType.at(Object_t::MITO);
	for (Object* mito : mitos)
	{
		double closest_distance = m_mesh_processing->compute_closest_distance(synapse, mito, m_mesh->getVerticesList());
		synapse->setDistanceToStructure(mito->getHVGXID(), closest_distance);
	}
}

void DataContainer::compute_closest_distance_to_structures()
{
	std::vector<Object*> mitos = m_objectsByType.at(Object_t::MITO);
	for (Object* mito : mitos)
	{
		compute_synapse_distances(mito);
		compute_mito_distances(mito);
	}

	std::vector<Object*> synapses = m_objectsByType.at(Object_t::SYNAPSE);
	for (Object* syn : synapses)
	{
		compute_mito_distances(syn);
	}
}

Object_t DataContainer::getObjectTypeByID(int hvgxID)
{
	Object* obj = m_objects[hvgxID]; // check if the ID is valid
	if (obj == NULL)
		return Object_t::UNKNOWN;

	return obj->getObjectType();
}

std::vector<Object*> DataContainer::getObjectsByType(Object_t type)
{
	return m_objectsByType[type];
}

std::string DataContainer::getObjectName(int hvgxID)
{
	if (hvgxID == 0)
		return "";//background
	if (m_objects.find(hvgxID) == m_objects.end())
		return "Unknown";

	return m_objects[hvgxID]->getName();
}

Object* DataContainer::getObject(int hvgxID)
{
	if (hvgxID > 0 && m_objects.count(1) == 1)
	{
		return m_objects.at(hvgxID);
	}
	else
	{
		return nullptr;
	}
}

Object* DataContainer::getObjectByName(QString name)
{
	for (auto const& [id, obj] : m_objects)
	{
		QString obj_name = QString::fromUtf8(obj->getName().c_str());
		if (name.contains(obj_name, Qt::CaseInsensitive))
		{
			return obj;
		}
	}
	return nullptr;
}

void DataContainer::addSliceVertices()
{
	Object* obj = new Object("Slice", -1);
	obj->setAstPoint(QVector4D(1.0f / MESH_MAX_X, 1.0f / MESH_MAX_Y, 1.0f / MESH_MAX_Z, 0));

	m_objects[-1] = obj;
	m_objectsByType[Object_t::SLICE].push_back(obj);

	int idx0 = addSliceVertex(0.0, 0.0, 0.0, 0.0);
	int idx1 = addSliceVertex(MESH_MAX_Y, 0.0, 0.0, 1.0);
	int idx2 = addSliceVertex(0.0, MESH_MAX_Z, 1.0, 0.0);
	int idx3 = addSliceVertex(MESH_MAX_Y, MESH_MAX_Z, 1.0, 1.0);

	m_mesh->addFace(idx0, idx1, idx2);
	m_mesh->addFace(idx3, idx1, idx2);

	obj->addTriangleIndex(idx0);
	obj->addTriangleIndex(idx1);
	obj->addTriangleIndex(idx2);

	obj->addTriangleIndex(idx3);
	obj->addTriangleIndex(idx1);
	obj->addTriangleIndex(idx2);
}

int DataContainer::addSliceVertex(float y, float z, float u, float v)
{
	std::vector< struct VertexData >* meshVertexList = m_mesh->getVerticesList();

	QVector4D mesh_vertex(u, y, z, v);

	meshVertexList->emplace_back();
	int vertexIdx = (int)meshVertexList->size() - 1;
	struct VertexData* vertex = &meshVertexList->at(vertexIdx);

	vertex->vertex = mesh_vertex;
	vertex->distance_to_cell = 0.0; // first init all distances with the default value
	vertex->hvgxID = -1;
	vertex->structure_type = (int)Object_t::SLICE;
	vertex->is_skeleton = false;

	return vertexIdx;
}
