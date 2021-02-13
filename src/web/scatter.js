new QWebChannel(qt.webChannelTransport, function (channel) {

    let jsobject = channel.objects.scatterplot_data;
    let json_string = jsobject.json_string;

    let xLabel = jsobject.x_axis_name;
    let yLabel = jsobject.y_axis_name;

    let xUnit = jsobject.x_unit;
    let yUnit = jsobject.y_unit;

    let data = JSON.parse(json_string);

    var margin = {top: 50, right: 200, bottom: 50, left: 200},
        outerWidth = 1000,
        outerHeight = 440,
        width = outerWidth - margin.left - margin.right,
        height = outerHeight - margin.top - margin.bottom;

    var x = d3.scale.linear()
        .range([0, width]).nice();

    var y = d3.scale.linear()
        .range([height, 0]).nice();

    var dendrite = "Dendrite";
    var axon = "Axon";

    var dend_color = '#5833ff';
    var axon_color = '#ff5733';

    var colorScale = d3.scale.ordinal()
        .domain([dendrite, axon])
        .range([dend_color, axon_color]);

    var min = "min",
        perc = "perc",
        name = "name";


    var xMax = d3.max(data, function (d) {
            return d[min];
        }) * 1.05,
        xMin = d3.min(data, function (d) {
            return d[min];
        }),
        xMin = xMin > 0 ? 0 : xMin,
        yMax = d3.max(data, function (d) {
            return d[perc];
        }) * 1.05,
        yMin = d3.min(data, function (d) {
            return d[perc];
        }),
        yMin = yMin > 0 ? 0 : yMin;

    x.domain([0.0, 0.3]);
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
            return d[name] + " <br>" + min + ": " + d[min].toFixed(4) + " " + xUnit +
                " <br>" + perc + ": " + d[perc].toFixed(4) + " " + yUnit;
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
        .text(xLabel + " in " + xUnit);

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
        .attr("r", 5)
        .attr("transform", transform)
        .style("fill", function (d) {
            if(d.name.includes("Mito_A"))
            {
                return colorScale(axon);
            }
            else if(d.name.includes("Mito_D"))
            {
                return colorScale(dendrite);
            }

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
        .attr("x", width + 26)
        .attr("dy", ".35em")
        .text(function (d) {
            return d;
        });

    function clicked(d) {
        jsobject.selectStructure(d.name);
    }

    function zoom() {
        svg.select(".x.axis").call(xAxis);
        svg.select(".y.axis").call(yAxis);

        svg.selectAll(".dot")
            .attr("transform", transform);
    }

    function transform(d) {
        return "translate(" + x(d[min]) + "," + y(d[perc]) + ")";
    }

});