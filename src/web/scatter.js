new QWebChannel(qt.webChannelTransport, function (channel) {

    let jsobject = channel.objects.scatterplot_data;
    let json_string = jsobject.json_string;
    let selected_structures_string = jsobject.selected_structures;

    let xLabel = jsobject.x_axis_name;
    let yLabel = jsobject.y_axis_name;

    let xUnit = jsobject.x_unit;
    let yUnit = jsobject.y_unit;

    let data = JSON.parse(json_string);

    console.log(data);

    let selected_structures = JSON.parse(selected_structures_string);

    var margin = {top: 50, right: 200, bottom: 50, left: 200},
        outerWidth = 1000,
        outerHeight = 440,
        width = outerWidth - margin.left - margin.right,
        height = outerHeight - margin.top - margin.bottom;

    var x = d3.scale.linear()
        .range([0, width]).nice();

    var y = d3.scale.linear()
        .range([height, 0]).nice();

    var mouse2 = "Mouse 2 (24 months)";
    var mouse3 = "Mouse 3 (4 months)";
    var selected = "Selected";

    var dend_identifier = "Mito_D";
    var axon_identifier = "Mito_A";

    var dend_color = '#cd15e8';
    var axon_color = '#e7d80e';
    var selected_color = "#358500";

    var radius = 3;

    var selected_opacity = 1.0;
    var unselected_opacity = 0.5;

    var colorScale = d3.scale.ordinal()
        .domain([mouse2, mouse3, selected])
        .range([dend_color, axon_color, selected_color]);

    var opacityScale = d3.scale.ordinal()
        .domain([mouse2, mouse3, selected])
        .range([unselected_opacity, unselected_opacity, selected_opacity]);

    var xAttrName = "x",
        yAttrName = "y",
        name = "name";


    var xMax = d3.max(data, function (d) {
            return d[xAttrName];
        }) * 1.05,
        xMin = d3.min(data, function (d) {
            return d[xAttrName];
        }),
        xMin = xMin > 0 ? 0 : xMin,
        yMax = d3.max(data, function (d) {
            return d[yAttrName];
        }) * 1.05,
        yMin = d3.min(data, function (d) {
            return d[yAttrName];
        }),
        yMin = yMin > 0 ? 0 : yMin;

    x.domain([xMin, xMax]);
    y.domain([yMin, yMax]);

    var xAxis = d3.svg.axis()
        .scale(x)
        .orient("bottom")
        .tickSize(-height);

    var yAxis = d3.svg.axis()
        .scale(y)
        .orient("left")
        .tickSize(-width);

    var tip = d3.tip()
        .attr("class", "d3-tip")
        .offset([-10, 0])
        .html(function (d) {
            return d[name] + " <br>" + xAttrName + ": " + d[xAttrName].toFixed(4) +
                " <br>" + yAttrName + ": " + d[yAttrName].toFixed(4);
        });

    var zoomBeh = d3.behavior.zoom()
        .x(x)
        .y(y)
        .scaleExtent([0, 500])
        .on("zoom", zoom);

    var svg = d3.select("#scatter")
        .append("svg")
        .attr("width", outerWidth)
        .attr("height", outerHeight)
        .append("g")
        .attr("transform", "translate(" + margin.left + "," + margin.top + ")")
        .call(zoomBeh);

    svg.call(tip);

    svg.append("rect")
        .attr("width", width)
        .attr("height", height);

    svg.append("g")
        .classed("x axis", true)
        .attr("transform", "translate(0," + height + ")")
        .call(xAxis)
        .append("text")
        .classed("label", true)
        .attr("x", width)
        .attr("y", margin.bottom - 10)
        .style("text-anchor", "end")
        .text(xLabel);

    svg.append("g")
        .classed("y axis", true)
        .call(yAxis)
        .append("text")
        .classed("label", true)
        .attr("transform", "rotate(-90)")
        .attr("y", -margin.left + 150)
        .attr("dy", ".71em")
        .style("text-anchor", "end")
        .text(yLabel);

    var objects = svg.append("svg")
        .classed("objects", true)
        .attr("width", width)
        .attr("height", height);

    objects.append("svg:line")
        .classed("axisLine hAxisLine", true)
        .attr("x1", 0)
        .attr("y1", 0)
        .attr("x2", width)
        .attr("y2", 0)
        .attr("transform", "translate(0," + height + ")");

    objects.append("svg:line")
        .classed("axisLine vAxisLine", true)
        .attr("x1", 0)
        .attr("y1", 0)
        .attr("x2", 0)
        .attr("y2", height);

    objects.selectAll(".dot")
        .data(data)
        .enter().append("circle")
        .classed("dot", true)
        .attr("r", radius)
        .attr("transform", transform)
        .style("fill", function (d) {
            return colorScale(decide_type(d.name));
        })
        .style("fill-opacity", function (d){
            return opacityScale(decide_type(d.name));
        })
        .on("click", function (d) {
            clicked(d)
        })
        .on("mouseover", tip.show)
        .on("mouseout", tip.hide);

    var legend = svg.selectAll(".legend")
        .data(colorScale.domain())
        .enter().append("g")
        .classed("legend", true)
        .attr("transform", function (d, i) {
            return "translate(0," + i * 20 + ")";
        });

    legend.append("circle")
        .attr("r", 3.5)
        .attr("cx", width + 20)
        .attr("fill", colorScale);

    legend.append("text")
        .classed("label", true)
        .attr("x", width + 26)
        .attr("dy", ".35em")
        .text(function (d) {
            return d;
        });

    function clicked(d) {
        selected_structures.push(d.name);
        highlightDots();
        jsobject.selectStructure(d.name);
    }

    function zoom() {
        svg.select(".x.axis").call(xAxis);
        svg.select(".y.axis").call(yAxis);

        svg.selectAll(".dot")
            .attr("transform", transform);
    }

    function transform(d) {
        return "translate(" + x(d[xAttrName]) + "," + y(d[yAttrName]) + ")";
    }

    function highlightDots() {
        objects.selectAll("circle")
            .style("fill", function (d) {
                return colorScale(decide_type(d.name));
            });
    }

    function decide_type(name)
    {
        if (selected_structures.includes(name)) {
            return selected;
        } else if (name.endsWith("_2")) {
            return mouse2;
        } else if (name.endsWith("_3")) {
            return mouse3;
        }
    }

});