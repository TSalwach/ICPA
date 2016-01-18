
function update_plot() {
    $.getJSON('data/data.php?callback=?', function (_data) {
        $('#plots').highcharts({
	    chart: {
        	type: 'line'
    	    },
	    plotOptions: {
            line: {
                lineWidth: 1,
                states: {
                    hover: {
                        lineWidth: 1
                    }
                },
                marker: {
                    enabled: false
                }
            }
    	    },
            title: {
                text: 'Voltage',
                x: -20
            },
	    yAxis: [{ 
	    labels: {
            	format: '{value}V',
            	style: {
                    color: Highcharts.getOptions().colors[1]
                }
            },
            title: {
                text: 'Voltage',
                style: {
                    color: Highcharts.getOptions().colors[1]
                }
            },
	    min: -400, max: 400,
    	    }, 

	    { 
            title: {
                text: 'Current',
                style: {
                    color: Highcharts.getOptions().colors[0]
                }
            },
            labels: {
                format: '{value} A',
                style: {
                    color: Highcharts.getOptions().colors[0]
                }
            },
	    min: -40, max: 40,
            opposite: true
           }],
            tooltip: {
                valueSuffix: 'V',
            },
            series: [{
                name: 'Ph1 (V)',
                data: _data.volt0,
		color: 'orange'
            },{
                name: 'Ph2 (V)',
                data: _data.volt1,
		color: 'green'
            },{
                name: 'Ph3 (V)',
                data: _data.volt2,
		color: 'blue'
            },{
                name: 'Ph1 (I)',
                data: _data.cur0,
		yAxis: 1,
		color: 'orange'
            },{
                name: 'Ph2 (I)',
                data: _data.cur1,
		yAxis: 1,
		color: 'green'
            },{
                name: 'Ph3 (I)',
                data: _data.cur2,
		yAxis: 1,
		color: 'blue'
            } ]
        });
/*
    $("#ph1").html(_data.rms_vol0+" V, " +_data.real_pwr0+" W, cos=" + (_data.real_pwr0/_data.apa_pwr0).toFixed(2));
    $("#ph2").html(_data.rms_vol1+" V, " +_data.real_pwr1+" W, cos=" + (_data.real_pwr1/_data.apa_pwr1).toFixed(2));
    $("#ph3").html(_data.rms_vol2+" V, " +_data.real_pwr2+" W, cos=" + (_data.real_pwr2/_data.apa_pwr2).toFixed(2));
    $("#tot").html((_data.real_pwr0+_data.real_pwr1+_data.real_pwr2).toFixed(0)+" W");
*/

    });



}
