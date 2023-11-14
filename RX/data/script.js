// Wait for the document to load
document.addEventListener("DOMContentLoaded", function () {
    // Function to update the data from the ESP8266
    function updateData() {
        // Make an AJAX request to your ESP8266 server to get the data
        fetch("/data")
            .then(response => response.text())
            .then(data => {
                // Update the content of the "data" element with the received data
                document.getElementById("data").textContent = data;
            })
            .catch(error => {
                console.error("Error fetching data:", error);
            });
    }

    // Call the updateData function initially
    updateData();

    // Set an interval to periodically update the data (e.g., every 5 seconds)
    setInterval(updateData, 500); // 5000 milliseconds = 5 seconds
});