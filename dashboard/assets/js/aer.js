var dataSize = 0;

window.onload = function () {
    var trigger = [];
    var imd = [];
    var sdc = [];
    var bspd = [];
    var mom1 = [];
    var mom2 = [];

    cnt = 0;
    var chart = new CanvasJS.Chart("chartContainer", {
        title :{
            text: "MCU Data"
        },
        legend: {
            cursor: "pointer",
            itemclick: function (e) {
                if (typeof (e.dataSeries.visible) === "undefined" || e.dataSeries.visible) {
                    e.dataSeries.visible = false;
                } else {
                    e.dataSeries.visible = true;
                }
 
                e.chart.render();
            }
        },
        axisY: {
            minimum: 0,
            maximum: 1.2
        },
        data: [
            {
                type: "line",
                name: "5v Trigger",
                visible: true,
                dataPoints: trigger,
                showInLegend: true
            },
            {
                type: "line",
                name: "IMD",
                visible: false,
                dataPoints: imd,
                showInLegend: true
            },
            {
                type: "line",
                name: "Shut Down Circuit",
                visible: false,
                dataPoints: sdc,
                showInLegend: true
            },
            {
                type: "line",
                name: "BSPD",
                visible: false,
                dataPoints: bspd,
                showInLegend: true
            },
            {
                type: "line",
                name: "MOM1",
                visible: false,
                dataPoints: mom1,
                showInLegend: true
            },
            {
                type: "line",
                name: "MOM2",
                visible: false,
                dataPoints: mom2,
                showInLegend: true
            }
        ]
    });
    
    var xVal = 0;
    var yVal = 100; 
    var updateInterval = 1000;
    var dataLength = 20; // number of dataPoints visible at any point

    var updateChart2 = function () {
    
        $.ajax({
            url: 'lib/graph.php',
            type: 'POST',
            dataType: 'json',
            success: function(response) { 
                var i = dataSize;
                var len = response.length;

                if (len <= dataSize) {
                    return;
                }


                while (i < len) {
                    if (response[i].length == 7) {
                        trigger.push({
                            x: cnt,
                            y: parseInt(response[i][1])
                        });
                        imd.push({
                            x: cnt,
                            y: parseInt(response[i][2])
                        })
                        sdc.push({
                            x: cnt,
                            y: parseInt(response[i][3])
                        })
                        bspd.push({
                            x: cnt,
                            y: parseInt(response[i][4])
                        })
                        mom1.push({
                            x: cnt,
                            y: parseInt(response[i][5])
                        })
                        mom2.push({
                            x: cnt,
                            y: parseInt(response[i][6])
                        })
                        i += 1;
                        cnt += 1;
                    }
                    else {
                        break;
                    }
                    
                }

                while (trigger.length > dataLength) {
                    trigger.shift();
                    imd.shift();
                    sdc.shift();
                    bspd.shift();
                    mom1.shift();
                    mom2.shift();
                }

                chart.render();

                i = dataSize;
                len = response.length;
                var table = document.getElementById("tableBody");

                while (i < len) {
                    if (response[i].length == 7) {
                        table.innerHTML = "<tr><td>" + response[i][0] + "</td><td>" + response[i][1] + "</td><td>" + response[i][2] + "</td><td>" + response[i][3] + "</td><td>" + response[i][4] + "</td><td>" + response[i][5] + "</td><td>" + response[i][6] + "</td></tr>" + table.innerHTML;
                        i += 1;
                    }
                    else {
                        break;
                    }
                    
                }

                dataSize = len;
            }
        });
    
    };
    
    updateChart2();
    setInterval(updateChart2, updateInterval);
}

function addRow() {
    
    table.innerHTML = "<tr><th scope='row'>" + iterations + "</th><td>" + timestamp + "</td></tr>"
    return true;
}
