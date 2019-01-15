Input Parameters
================


Parameter File
==============

Parameter File Generator
========================

Here the you can directly edit your own parameter file.



  .. <script>
  ..   //var filename = "./_static/seed.inp";
  ..   //var fileblob = new Blob([filename], {type : 'text/plain'});
  ..   //var reader = new FileReader();
  ..   //reader.onload = function (e) {
  ..   //  var textArea = document.getElementById("param_area");
  ..   //  textArea.value = e.target.result;
  ..   //};
  ..   //reader.addEventListener("loadend", function() {
  ..     // reader.result contains the contents of blob as a typed array
  ..   //});
  ..   //reader.readAsText(fileblob);
  ..   var xhr = new XMLHttpRequest();
  ..   xhr.open("GET", "./_static/seed.inp");
  ..   //although we can get the remote data directly into an arraybuffer
  ..   //using the string "arraybuffer" assigned to responseType property.
  ..   //For the sake of example we are putting it into a blob and then copying the blob data into an arraybuffer.
  ..   xhr.responseType = "blob";
  ..
  ..   function analyze_data(blob)
  ..   {
  ..       var myReader = new FileReader();
  ..       myReader.readAsText(blob)
  ..
  ..       var textArea = document.getElementById("param_area");
  ..       textArea.value = "sticazzi"//e.srcElement.result;
  ..
  ..       myReader.addEventListener("loadend", function(e)
  ..       {
  ..           //var buffer = e.srcElement.result;//arraybuffer object
  ..           //var textArea = document.getElementById("param_area");
  ..           //textArea.value = "sticazzi"//e.srcElement.result;
  ..       });
  ..   }
  ..
  ..   xhr.onload = function()
  ..   {
  ..       //analyze_data(xhr.response);
  ..       document.getElementsByTagName("body")[0].innerHTML = xhr.response;
  ..   }
  ..   xhr.send();
  .. </script>

.. raw:: html

  <textarea id="new-projects">

  </textarea>
  <!-- change div to textarea if you want to load your file in textarea-->
  <script type="text/javascript">
  jQuery(document).ready(function(){
    jQuery( "#new-projects" ).load("_static/seed.inp")
  });
  </script>

elabanda
