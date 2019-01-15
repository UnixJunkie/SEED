Input Parameters
================


Parameter File
==============

Parameter File Generator
========================

Here you can directly edit your own parameter file and download it.

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

  <textarea id="input-area" cols="100" style="resize:none" rows="20">

  </textarea>
  <!-- change div to textarea if you want to load your file in textarea-->
  <script type="text/javascript">
  jQuery(document).ready(function(){
    jQuery( "#input-area" ).load("_static/seed.inp");
  });
  </script>
  
  <div class="form-group">
    <label for="input-fileName">File name</label>
    <input type="text" class="form-control" id="input-fileName" value="seed.inp" placeholder="Enter file name">
  </div>
  
  <div>
    <button id="btn-save">Save input file</button>
  </div>
  
  <script src="./_static/FileSaver.js"></script>
  <script>
  jQuery("#btn-save").click( function(){
    var text = jQuery("#input-area").val();
    var filename = jQuery("#input-fileName").val()
    var blob = new Blob([text], {type: "text/plain;charset=utf-8"});
    saveAs(blob, filename);
  });
  </script>
  
  
  
