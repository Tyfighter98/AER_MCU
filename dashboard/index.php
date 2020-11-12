<!DOCTYPE HTML>
<html>
	<head>
		<script src="dist/canvasjs.min.js"></script>
		<script src="dist/jquery/jquery-3.5.1.min.js"></script>
		<link rel="stylesheet" href="dist/bootstrap/css/bootstrap.min.css">
		<script src='dist/bootstrap/js/bootstrap.min.js'></script>
		<script src="assets/js/aer.js"></script>
	</head>
	<body>
		<div id="chartContainer" style="height: 370px; width:100%;"></div>
		<table class="table" id="table">
			<thead>
				<tr>
				<th scope="col">Time</th>
				<th scope="col">5v Trigger</th>
				<th scope="col">IMD</th>
				<th scope="col">Shut Down Circuit</th>
				<th scope="col">BSPD</th>
				<th scope="col">MOM1</th>
				<th scope="col">MOM2</th>
				</tr>
			</thead>
			<tbody id="tableBody">
			</tbody>
		</table>
	</body>
</html>