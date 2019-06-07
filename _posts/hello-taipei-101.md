---
layout: page
published: true
---

<style>
  /* Always set the map height explicitly to define the size of the div
   * element that contains the map. */
  #map {
    height: 100%;
  }
  /* Optional: Makes the sample page fill the window. */
  html, body {
    height: 100%;
    margin: 0;
    padding: 0;
  }
</style>
<div id="map"></div>
<script>
  var map;
  function initMap() {
    map = new google.maps.Map(document.getElementById('map'), {
      center: {lat: 25.0339639, lng: 121.5622835}, // Taipei 101
      zoom: 16	// zoom bigger
    });
  }
</script>
<script src="https://maps.googleapis.com/maps/api/js?callback=initMap"
async defer></script>