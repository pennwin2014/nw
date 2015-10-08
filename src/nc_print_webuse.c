<html>
<head>
<title></title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel="stylesheet" href="/css.css" type="text/css">
<script>
function goto(t){
self.location.href=t
}
function lprint(){
document.all.pprint.style.display="none"
  window.print()

}
</script>
<script language="JavaScript">
<!--

function MM_reloadPage(init) {  //reloads the window if Nav4 resized
  if (init==true) with (navigator) {if ((appName=="Netscape")&&(parseInt(appVersion)==4)) {
    document.MM_pgW=innerWidth; document.MM_pgH=innerHeight; onresize=MM_reloadPage; }}
  else if (innerWidth!=document.MM_pgW || innerHeight!=document.MM_pgH) location.reload();
}
MM_reloadPage(true);


function MM_findObj(n, d) { //v4.0
  var p,i,x;  if(!d) d=document; if((p=n.indexOf("?"))>0&&parent.frames.length) {
    d=parent.frames[n.substring(p+1)].document; n=n.substring(0,p);}
  if(!(x=d[n])&&d.all) x=d.all[n]; for (i=0;!x&&i<d.forms.length;i++) x=d.forms[i][n];
  for(i=0;!x&&d.layers&&i<d.layers.length;i++) x=MM_findObj(n,d.layers[i].document);
  if(!x && document.getElementById) x=document.getElementById(n); return x;
}

function MM_showHideLayers() { //v3.0
  var i,p,v,obj,args=MM_showHideLayers.arguments;
  for (i=0; i<(args.length-2); i+=3) if ((obj=MM_findObj(args[i]))!=null) { v=args[i+2];
    if (obj.style) { obj=obj.style; v=(v=='show')?'visible':(v='hide')?'hidden':v; }
    obj.visibility=v; }
}
//-->
</script>

</head>
      <table width="75%" border="0" align="center">
        <tr> 
          <td height="31"> 
            <div id="plabel"> </div>
            <div id="mybut" align="center">
        <img id="pprint" src="/images/print2.gif" onClick="lprint()"> 
            </div>
          </td>
        </tr>
      </table>
  <!-- [#SLPB#] -->
      
<table border="0" align="center" cellpadding="0" cellspacing="0" width="84%" bgcolor="#FFFFFF">
  <tr> 
    <td colspan="6"> 
      <table width="100%" border="0">
        <tr> 
          <td> 
            <p align="center"><b>网站访问情况明细表</b></p>
            <p align="center">([#sdate#]--[#edate#]) </p>
          </td>
        </tr>
      </table>
    </td>
  </tr>
  <tr> 
    <td colspan="6">&nbsp; </td>
  </tr>
  <tr align="center" valign="middle"> 
    <td width="7%"  class="bord2" height="20"> 
      <div align="center"><b>序号</b></div>
    </td>
    <td  class="bord2" height="20"  width="13%"> 
      <div align="center"><b>用户名</b></div>
    </td>
    <td  class="bord2" height="20" width="32%"> 
      <div align="center"><b>访问站点</b></div>
    </td>
    <td  class="bord2" height="20" width="16%"> 
      <div align="center"><b>访问流量(Bytes)</b></div>
    </td>
    <td  class="bord2" height="20"  width="14%"> 
      <div align="center"><b>访问次数</b></div>
    </td>
    <td  class="bord4" height="20"  width="18%" >
      <div align="center"><b>访问时间</b></div>
    </td>
  </tr>
  <!-- [#SLPB#] -->
  <tr bgcolor="#FFFFCC"> 
    <td width="7%"  class="bord1" height="10"> 
      <div align="center"><b>&nbsp; [#num#]</b></div>
    </td>
    <td  class="bord1" height="10"  width="13%"><b>&nbsp; [#username#]</b></td>
    <td  class="bord1" height="10" width="32%"> 
      <div align="center"> <b>&nbsp;</b></div>
    </td>
    <td  class="bord1" height="10" width="16%"> 
      <div align="right"><b>[#lbytes1#]&nbsp;</b></div>
    </td>
    <td  class="bord1" height="10" width="14%"> 
      <div align="right"><b>[#lcount1#]&nbsp;</b></div>
    </td>
    <td  class="bord3" height="10" width="18%"> 
      <div align="right"><b>[#ltime1#]&nbsp;</b></div>
    </td>
  </tr>
  <!-- [#SLPB#] -->
  <tr> 
    <td width="7%"  class="bord1" height="10">&nbsp;</td>
    <td  class="bord1" height="10"  width="13%">&nbsp;</td>
    <td  class="bord1" height="10" width="32%">[#host#]&nbsp;</td>
    <td  class="bord1" height="10" width="16%"> 
      <div align="right">[#lbytes#]&nbsp;</div>
    </td>
    <td  class="bord1" height="10" width="14%"> 
      <div align="right">[#lcount#]&nbsp;</div>
    </td>
    <td  class="bord3" height="10" width="18%"> 
      <div align="right">[#ltime#]&nbsp;</div>
    </td>
  </tr>
  <!-- [#SLPE#] -->
  <!-- [#SLPE#] -->
</table>
	  
<p align="center">第 [#pages#]页</p>
<xmp style='page-break-after:always'></xmp>
<!-- [#SLPE#] -->
            
          
<p>&nbsp;</p>
</body>
</html>
