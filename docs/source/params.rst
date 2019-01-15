Input Parameters
================

Here we define all the parameters of the ``seed.inp`` file.

**i1**
  name of parameter file (``seed.par``) 
  
**i2**
  name of coordinate file for the receptor (in mol2 format)

**i3**
  | Binding site residue list.
  | First line: number of residues in the list
  | Following lines: residue number (one per line, according to the numbering in 
    the receptor input file).
  
**i4**
  | List of points (*e.g.* ligand heavy atoms of a known ligand-receptor 
    complex structure) in the binding site used to select polar and apolar 
    rec. vectors which satisfy the angle criterion (see parameters file).
  | First line:  number of points (``0``: no removal of vectors using the angle criterion).
  | Following lines: coordinates of the points (one point per line).

**i5**
  | Metals in the binding site.
  | Make sure that the residue number of the metal is in the
    binding site residue list. 
  | First line:  total number of coordination points.
  | Following lines:  atom number of metal / x y z of coordination point
  
**i6**
  Spherical cutoff for docking (y,n / sphere center / sphere radius)
  
**i7**
  | Fragment library specifications
  | First line: run specification: dock+energy (``d``) or only energy (``e``)
  | Following lines: Fragment library filename / 
    apolar docking, polar docking, or both (``a``, ``p``, ``b``) /
    energy cutoff in kcal/mol / 2nd clustering cutoff in kcal/mol
    
As you do not need to modify all the parameters and in most of the cases 
default values will give good results, we recommend not to write an input 
file from scratch, but to modify a default template. You can do this here through 
the :ref:`par_generator`.
  
Parameter File
==============

Here we define all the parameters of the ``seed.par`` file.

**p1**
  Dielectric constant of the solute (receptor and fragment)
  
**p2**
  Ratio of kept vectors for docking : polar / apolar

**p3**
  | Output control for structure files (two values on the same line).
  | First value: write \*_clus.mol2 file (y/n)
  | Second value: write \*_best.mol2 file (y/n)

**p4**
  | Output control for energy table files (two values on the same line).
  | First value: write \*_clus.dat summary table file (y/n)
  | Second value: write \*_best.dat summary table file (y/n)

**p5**
  | Maximum number of saved clusters and poses (two values on the same line).
  | First value: maximum number of cluster members saved in \*_clus\* 
    output files. Note that this value determines the maximum number
    of poses per cluster that go through slow energy evaluation.
    Second value: maximum number of poses saved in \*_best\* output files.

**p6**
  | Filename for output log file. This is the main SEED output file (``seed.out``).
  | The docked fragments are saved in the directory ./outputs         

**p7**
  write (w) or read (r) Coulombic grid / grid filename

**p8**
  write (w) or read (r) van der Waals grid / grid filename

**p9**
  write (w) or read (r) receptor desolvation grid / grid filename 

**p10**

**p11**

**p12**

**p13**

**p14**

**p15**

**p16**

**p17**

**p18**

**p19**

**p20**

**p21**

**p22**

**p23**

**p24**

**p25**

**p26**

**p27**

**p28**

**p29**

**p30**

**p31**

.. _par_generator:

Parameter File Generator
========================

Here you can directly edit a default input file and download it.

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
  
  
  
