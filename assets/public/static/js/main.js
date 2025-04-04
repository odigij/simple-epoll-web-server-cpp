function fetchData(url, onSuccess, onError) {
	$.ajax({
		url: url,
		method: "GET",
		dataType: "json",
		success: function (response) {
			if (typeof onSuccess === "function") onSuccess(response);
		},
		error: function (xhr, status, error) {
			console.error(`[AJAX ERROR] ${status}: ${error}`);
			if (typeof onError === "function") onError(xhr, status, error);
		},
	});
}

function fetchAllRoutes() {
	fetchData(
		"/test/routes",
		function (response) {
			$("#routes-list").empty();
			for (const route of response.routes || []) {
				$("#routes-list").append(`<li>${route.method} ${route.path}</li>`);
			}
		},
		function () {
			$("#routes-list").html("<li>Error fetching routes</li>");
		}
	);
}

function fetchDynamicRoute(id, type) {
	fetchData(
		`/test/routes/dynamic/${id}/${type}`,
		function (response) {
			$("#dynamic-output").text(JSON.stringify(response, null, 2));
		},
		function () {
			$("#dynamic-output").text("Failed to fetch dynamic route.");
		}
	);
}

$(document).ready(function () {
	fetchAllRoutes();

	$("#load-dynamic-btn").on("click", function () {
		const id = $("#input-id").val();
		const type = $("#input-type").val();
		fetchDynamicRoute(id, type);
	});
});
