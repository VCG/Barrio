new QWebChannel(qt.webChannelTransport, function (channel) {
    // Set global Qt proxy object
    var jsobject = channel.objects.distance_tree_data;
    let data = jsobject.newickString;

    let margin = {top: 20, right: 40, bottom: 10, left: 10};
    let width = $("#tree_display").width() - margin.left - margin.right;
    let height = $("#tree_display").height() - margin.top - margin.bottom;

    var tree;

    function edgeStyler(dom_element, edge_object) {
        dom_element.on('mouseenter', function() {
            div.transition()
                .duration(200)
                .style("opacity", .9);

            div.text(edge_object.target.name + ": " + edge_object.target.attribute)
                .style("left", (d3.event.pageX) + "px")
                .style("top", (d3.event.pageY - 28) + "px");
        });

        dom_element.on('mouseleave', function() {
            div.transition()
                .duration(500)
                .style("opacity", 0);
        });

    }

    function nodeStyler(dom_element, node_object) {

        var circle = dom_element.selectAll("circle");
        if (node_object.name.includes("Syn")) {
            circle.style("fill", "rgb(184,47,53)");
        }
        if (node_object.name.includes("Mito")) {
            circle.style("fill", "#354cc0");
        }

        dom_element.on('mouseenter', function() {
            div.transition()
                .duration(200)
                .style("opacity", .9);

            div.text(node_object.name)
                .style("left", (d3.event.pageX) + "px")
                .style("top", (d3.event.pageY - 28) + "px");
        });

        dom_element.on('mouseleave', function() {
            div.transition()
                .duration(500)
                .style("opacity", 0);
        });

        if ("bootstrap" in node_object && node_object.bootstrap) {
            var label = dom_element.selectAll(".bootstrap");
            if (label.empty()) {
                dom_element.append("text").classed("bootstrap", true).text(node_object.bootstrap).attr("dx", ".10em").attr("text-anchor", "start").attr("alignment-baseline", "top");
            } else {
                if (tree.radial()) { // do not show internal node labels in radial mode
                    label.remove();
                }
            }

        }
    }

    function selectNode() {
        tree.modify_selection(function (d) {
            if (jsobject.new_selected_node == d.target.name) {
                return true;
            }
            return false;
        });
    }

    function drawATree(data) {

        tree = d3.layout.phylotree()
            .svg(d3.select("#tree_display"))
            .options({
                'selectable': true,
                'collapsible': false,
                'left-right-spacing': 'fit-to-size',
                'top-bottom-spacing': 'fixed-step',
                'zoom': false,
                'align-tips': false,
                'show-scale': true,
                'brush': true
            })
            .size([height, width])
            .style_edges(edgeStyler)
            .style_nodes(nodeStyler)
            .node_circle_size(8) // do not draw clickable circles for internal nodes
            .branch_name(function() {
                return ""; // no leaf names
            });
        /* the next call creates the tree object, and tree nodes */
        tree(d3.layout.newick_parser(data));

        tree.spacing_x(40);

        if ($("#layout").prop("checked")) {
            tree.radial(true);
        }
        tree.placenodes().layout();

        tree.selection_callback(selection => {
            jsobject.removeAllHighlightedStructures();
            var names = selection.map(d => d.name);
            names.forEach(function (name) {
                jsobject.setHighlightedStructure(name);
            });
        });


        // UI handlers
        $("#layout").on("click", function (e) {
            tree.radial($(this).prop("checked")).placenodes().update();
        });
    }

    // Append Div for tooltip to SVG
    let div = d3.select("body")
        .append("div")
        .attr("class", "tooltip")
        .style("opacity", 0);

    drawATree(data);
});