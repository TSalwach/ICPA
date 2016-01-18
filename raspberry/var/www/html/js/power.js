var gauges;
var total;

function create_power() {

$.getJSON('data/data_5min.php?callback=?', function (_data) {

    $('#total').highcharts({
            chart: {
                type: 'spline',
                //animation: Highcharts.svg,
                marginRight: 10,
            },
            title: {
                text: ""
            },
	    plotOptions: {
            spline: {
                lineWidth: 2,
                states: {
                    hover: {
                        lineWidth: 2
                    }
                },
                marker: {
                    enabled: false
                }
            }
            },

            xAxis: {
                type: 'datetime',
                tickPixelInterval: 150
            },
            yAxis: {
                title: {
                    text: 'Power'
                },
		minTickInterval: 100,
                plotLines: [{
                    value: 0,
                    width: 1,
                    color: '#808080'
                }]
            },
            tooltip: {
                formatter: function () {
                    return '<b>' + this.series.name + '</b><br/>' +
                        Highcharts.dateFormat('%Y-%m-%d %H:%M:%S', this.x) + '<br/>' +
                        Highcharts.numberFormat(this.y, 2);
                }
            },
            legend: {
                enabled: false
            },
            exporting: {
                enabled: false
            },
            series: [{
                name: 'Total power',
		data: _data
            }]
    },function (chart) {
	    total=chart;  
    });
});


     $('#gauges').highcharts({
        chart: {
            type: 'gauge',
            plotBorderWidth: 1,
            plotBackgroundColor: {
                linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1 },
                stops: [
                    [0, '#FFF4C6'],
                    [0.3, '#FFFFFF'],
                    [1, '#FFF4C6']
                ]
            },
            plotBackgroundImage: null,
            height: 200
        },

        title: {
            text: 'RMS currents'
        },
        pane: [{
            startAngle: -45,
            endAngle: 45,
            background: null,
            center: ['17%', '145%'],
            size: 280
        }, {
            startAngle: -45,
            endAngle: 45,
            background: null,
            center: ['50%', '145%'],
            size: 280
        },{
            startAngle: -45,
            endAngle: 45,
            background: null,
            center: ['83%', '145%'],
            size: 280
        }],
        tooltip: {
            enabled: false
        },

        yAxis: [{
            min: 0,
            max: 30,
            minorTickPosition: 'outside',
            tickPosition: 'outside',
            labels: {
                rotation: 'auto',
                distance: 20
            },
            plotBands: [{
                from: 20,
                to: 30,
                color: '#C02316',
                innerRadius: '100%',
                outerRadius: '105%'
            }],
            pane: 0,
            title: {
                text: '',
                y: -40
            }
        }, {
            min: 0,
            max: 30,
            minorTickPosition: 'outside',
            tickPosition: 'outside',
            labels: {
                rotation: 'auto',
                distance: 20
            },
            plotBands: [{
                from: 20,
                to: 30,
                color: '#C02316',
                innerRadius: '100%',
                outerRadius: '105%'
            }],
            pane: 1,
            title: {
                text: '',
                y: -40
            }
        },{
            min: 0,
            max: 30,
            minorTickPosition: 'outside',
            tickPosition: 'outside',
            labels: {
                rotation: 'auto',
                distance: 20
            },
            plotBands: [{
                from: 20,
                to: 30,
                color: '#C02316',
                innerRadius: '100%',
                outerRadius: '105%'
            }],
            pane: 2,
            title: {
                text: '',
                y: -40
            }
        }],

        plotOptions: {
            gauge: {
                dataLabels: {
                    enabled: false
                },
                dial: {
                    radius: '100%'
                }
            }
        },


        series: [{
            name: 'Phase 1',
            data: [0],
            yAxis: 0
        }, {
            name: 'Phase 2',
            data: [0],
            yAxis: 1
        }, {
            name: 'Phase 3',
            data: [0],
            yAxis: 2
        }]

    }, function (chart) {
	    gauges=chart;  
    });
}

function update_power(){

$.getJSON('data/data.php?live=1&callback=?', function (_data) {
	        if (gauges.series){
			gauges.series[0].points[0].update(_data.rms_cur0, false);
			gauges.series[1].points[0].update(_data.rms_cur1, false);
			gauges.series[2].points[0].update(_data.rms_cur2, false);
			gauges.yAxis[0].setTitle({ text: _data.rms_cur0+" A"},false);
			gauges.yAxis[1].setTitle({ text: _data.rms_cur1+" A"},false);
			gauges.yAxis[2].setTitle({ text: _data.rms_cur2+" A"},false);
                	gauges.redraw();
		}
		if (total) if (total.series){
		    var len=total.series[0].data.length;
		    var tot=_data.real_pwr0+_data.real_pwr1+_data.real_pwr2;
		    //total.setTitle({ text: "Power: "+tot.toFixed(0) +" W" },"",false);
		    if (len==1) total.series[0].addPoint([ new Date().getTime(), tot], true, true);
		    if (len>300) total.series[0].addPoint([ new Date().getTime(), tot], true, true);
		    else total.series[0].addPoint([ new Date().getTime(), tot], true, false);
		}
		var tb="<table class='dtable'><tr><th>Ph</th><th>Volt(RMS)</th><th>Cur(RMS)</th><th>Real</th><th>Reactive</th><th>Apparent</th><th>cos &phi;</th></tr>";
		for (var i=0;i<3;i++){
		    tb+="<tr><td>"+(i+1)+"</td><td>"+(_data['rms_vol'+i]).toFixed(1)+" V</td>";
		    tb+="<td>"+(_data['rms_cur'+i]).toFixed(2)+" A</td>";
		    tb+="<td>"+(_data['real_pwr'+i]).toFixed(1)+" W</td>";
		    tb+="<td>"+(_data['react_pwr'+i]).toFixed(1)+" VAR</td>";
		    tb+="<td>"+(_data['apa_pwr'+i]).toFixed(1)+" VA</td>";
		    tb+="<td>"+(_data['real_pwr'+i]/_data['apa_pwr'+i]).toFixed(2)+"</td>";
		    tb+="</tr>";
		}
		tb+="<tr><td>&nbsp;</td></tr><tr><td colspan=3>Total:</td><td>"+(_data['pwr']).toFixed(1)+" W</td><td colspan=2></td><td>"+(_data['freq']).toFixed(3)+" Hz</td></tr>";
		var tar0="",tar1="";
		if (_data.tariff==0) tar0="&rarr;"; else tar1="&rarr;";
		tb+="<tr><td colspan=2>Peak"+tar0+"</td><td colspan=3>"+(_data['tar0']).toFixed(3)+" kWh</td></tr>";
		tb+="<tr><td colspan=2>Off-peak"+tar1+"</td><td colspan=3>"+(_data['tar1']).toFixed(3)+" kWh</td></tr>";


		tb+="</table>";
		$('#table').html(tb);

	});
}




