let map;
let lan=0;
let lon=0;
function send(fence_val) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("fence_id").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "status?fenceValue="+fence_val, true);
    xhttp.send();
  }

  setInterval(function() {getData();}, 2000); 

  function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        const data = this.responseText.split("#");
        document.getElementById("date").innerHTML = data[1];
        document.getElementById("sats").innerHTML = data[0];
        document.getElementById("lat_val").innerHTML = data[2];
        lan = data[2];
        document.getElementById("lon_val").innerHTML = data[3];
        lon = data[3];
        document.getElementById("target_status").innerHTML = data[4];
        document.getElementById("angle").innerHTML = data[5];
        document.getElementById("Device_status").innerHTML = data[6];             
      }
    };
    xhttp.open("GET", "values", true);
    xhttp.send();
    initMap();
  }


function initMap() {
  map = new google.maps.Map(document.getElementById("map"), {
    center: { lat: lan, lng: lon },
    zoom: 8,
  });
}
window.initMap = initMap;

