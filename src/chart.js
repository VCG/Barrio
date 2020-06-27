new QWebChannel(qt.webChannelTransport, function (channel) {
    // Set global Qt proxy object
    var jsobject = channel.objects.data;

    var values = jsobject.values;
    var labels = jsobject.labels;

    console.log(values);
    console.log(labels);

    var margin = {top: 10, right: 10, bottom: 30, left: 30},
        width = $("#barchart").width() - margin.left - margin.right,
        height = $("#barchart").height() - margin.top - margin.bottom;

    console.log("width: " + width + ", height: " + height);
    
    var svg = d3.select("body").append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
        .append("g")
        .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

    var x = d3.scaleBand().rangeRound([0, width]).padding(0.1),
        y = d3.scaleLinear().rangeRound([height, 0]);

    var g = svg.append("g")
        .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

    var values = [0.7, 0.8, 0.9];
    var labels = ["dend1", "dend2", "dend3"];
    var data = [];

    values.forEach(function (v, index) {
        data.push({name: labels[index], value: v});
    })

    console.log(data);

    x.domain(data.map(function (d) {
        return d.name;
    }));
    y.domain([0, d3.max(data, function (d) {
        return d.value
    })]);

    g.append("g")
        .attr("class", "axis axis--x")
        .attr("transform", "translate(0," + height + ")")
        .call(d3.axisBottom(x));

    g.append("g")
        .attr("class", "axis axis--y")
        .call(d3.axisLeft(y))
        .append("text")
        .attr("transform", "rotate(-90)")
        .attr("y", 6)
        .attr("dy", "0.71em")
        .attr("text-anchor", "end")
        .text("Distance");

    g.selectAll(".bar")
        .data(data)
        .enter().append("rect")
        .attr("class", "bar")
        .attr("x", function (d) {
            return x(d.name);
        })
        .attr("y", function (d) {
            return y(d.value);
        })
        .attr("width", x.bandwidth())
        .attr("height", function (d) {
            return height - y(d.value);
        });
});


// // Pass user name and password to QT
// // Note: you can call this function on button click.
// function toQT(name){
//    qProxy.setData(name);
// }