function freq_today() {
    $.getJSON('data/data_freq.php?callback=?', function (_data) {

        $('#freq').highcharts({
            chart: {
                zoomType: 'x',
		type: 'line'
            },
            title: {
                text: 'Frequency '
            },
            xAxis: {
                type: 'datetime'
            },
            yAxis: {
                title: {
                    text: 'Frequency'
                },
		labels: {
            	    format: '{value} Hz'
		},
		min: 49.8,
		max: 50.2
            },
            legend: {
                enabled: false
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
	     series: [{
                type: 'line',
                name: 'Frequency',
                data: _data
            }]

        });
    });
}
