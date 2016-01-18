function pwr_today() {
    $.getJSON('data/data_pwr.php?callback=?', function (_data) {

        $('#pwr_today').highcharts({
            chart: {
                zoomType: 'x',
		type: 'area'
            },
            title: {
                text: 'Power '
            },
            xAxis: {
                type: 'datetime'
            },
            yAxis: {
                title: {
                    text: 'Power'
                },
		min: 0
            },
            legend: {
                enabled: false
            },
	    
            plotOptions: {
		line: {
		    marker: {
			enabled: false
		    }
		},
                area: {
                    fillColor: {
                        linearGradient: {
                            x1: 0,
                            y1: 0,
                            x2: 0,
                            y2: 1
                        },
                        stops: [
                            [0, Highcharts.getOptions().colors[0]],
                            [1, Highcharts.Color(Highcharts.getOptions().colors[0]).setOpacity(0).get('rgba')]
                        ]
                    },
                    marker: {
                        radius: 0
                    },
                    lineWidth: 1,
                    states: {
                        hover: {
                            lineWidth: 1
                        }
                    },
                    threshold: null
                }
            },

	     series: [{
                type: 'area',
                name: 'Power',
                data: _data[0]
            },{
                type: 'line',
                name: 'ph1',
                data: _data[1]
            },{
                type: 'line',
                name: 'ph2',
                data: _data[2]
            },{
                type: 'line',
                name: 'ph3',
                data: _data[3]
            }]

        });
    });
}
