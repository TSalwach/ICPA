<?php
    function print_js_file($file){
	print("<script type=\"text/javascript\" charset=\"utf-8\">\n");
	print(file_get_contents($file));
	print("</script>");
    }

    ob_start("ob_gzhandler");
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
    <head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
	<title>Power meter</title>
	<script type="text/javascript" src="lib/jquery.min.js"></script>
	<script type="text/javascript" src="lib/jquery-ui.min.js"></script>
	<script type="text/javascript" src="lib/jquery.imagemapster.min.js"></script>
	<script type="text/javascript" src="lib/highcharts.js"></script>
	<script type="text/javascript" src="lib/highcharts-more.js"></script>
<?php
    print_js_file("js/power.js");
    print_js_file("js/plot.js");
    print_js_file("js/pwr_today.js");
    print_js_file("js/freq_today.js");
?>

	<link rel="stylesheet" href="lib/jquery-ui.min.css">
	<link rel="stylesheet" href="lib/theme.css">
	<style>
	
	.wui-tabs .wui-tabs-panel {
	    position:relative;
	}
	#table table{
	    width: 100%;
	    margin: 1px;
	    border: 1px solid black;
	    border-collapse: collapse;
	    text-align: left;
	}
	#ene_month table{
	    width: 600px;
	    margin: 1px;
	    border: 1px solid black;
	    border-collapse: collapse;
	    text-align: left;
	}
	.weekend {
	    background: #DDE;
	}

	</style>
	<script type="text/javascript">


	    Highcharts.setOptions({
	        global: {   useUTC: false }
	    });

	    $(function () { // On DOM ready...#
		$( "#tabs" ).tabs({
		    activate: function( event, ui ) {
			var tab=ui.newPanel.attr('id');
			if (tab=='plot_tab') update_plot();
			if (tab=='pwr_tab') pwr_today();
			if (tab=='freq_tab') freq_today();
			if (tab=='ene_tab') {
			    $("#ene_month").load("data/data_ene.php");
			}
		    }
		});
		
		create_power();
		update_power();
		setInterval(update_power, 5000);

	    });
	</script>
    </head>
    <body>

    <div id="tabs">
  <ul>
    <li><a href="#power_tab">Power</a></li>
    <li><a href="#pwr_tab">PWR today</a></li>
    <li><a href="#ene_tab">Energy</a></li>
    <li><a href="#plot_tab">Plots</a></li>
    <li><a href="#freq_tab">Freq</a></li>
  </ul>

  <div id="power_tab">
    <div id="gauges" style="width: 800px; height: 200px; margin: 0 auto"></div>
    <div id="table"  style="width: 800px;  margin: 0 auto"></div>
    <div id="total"  style="width: 800px; height: 300px; margin: 0 auto"></div>
  </div>

  <div id="pwr_tab">
     <div id="pwr_today" style="width: 90%; height: 90%; margin: 0 auto"></div>
  </div>

  <div id="ene_tab">
     <div id="ene_month" style="width: 90%; height: 90%; margin: 0 auto"></div>
  </div>

  <div id="plot_tab">
     <div id="plots" style="width: 90%; height: 90%; margin: 0 auto"></div>
  </div>
  <div id="freq_tab">
     <div id="freq" style="width: 90%; height: 90%; margin: 0 auto"></div>
  </div>


</div>
    </body>
</html>
