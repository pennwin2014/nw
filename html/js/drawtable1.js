
var hRH=22;
var bRH=18;
var rAMW=10;
var cBW=1;
var aTRLC=true;
var tVS="auto";
var tHS="auto";
var tRABL="#CC0000 1px solid";
var tRABR="black 1px solid";
var tRABT="";var tRABB="";
var tRAB="";var tRAO="";
var tHB="#6382BD";
var rBH="";var rTH="";
var sRTD="Drag To Resize";
var rCO=0;
var rCI=0;

function FlexibleTable ( TID, tW, tH, tA, fRN, oT, dT ){
    this.tW=tW;
    this.tH=tH;
    this.tA=tA;
    this.fRN=(fRN)?fRN:0;
    this.dT=(dT)?dT:'document';
    this.oT=oT;
    this.TID=TID;
    this.hCs=new Array ();
    this.CsWh1=new Array ();
    this.hCsAt=new Array ();
    this.bCat0=new Array ();
    this.Dsr7=new Array ();
    this.hddCs=new Array ();
    this.csTes=new Array ();
    this.rsTe='';
    this.Draw=fvm409gjskldjr;
    this.Header=fvm590gdjfv;
    this.ColumnsWidth=fdjklfbxv;
    this.HeaderAligment=hCsAt;
    this.BodyAligment=evbfbdd;
    this.Row=vckdd8df;
    this.RowsCount=vrgf4gl1;
    this.ColumnsCount=bvdf4gf;
    this.NewDataSource=blkkun;
    this.SetColumnWidth=SetColumnWidth;
    this.ColumnWidthTruncation=ColumnWidthTruncation;
    this.SetColumnTemplate=SetColumnTemplate;
    this.SetRowsTemplate=SetRowsTemplate;
    this.UnsetColumnsTemplates=UnsetColumnsTemplates;
    this.UnsetRowsTemplate=UnsetRowsTemplate;
    this.UnsetTemplates=UnsetTemplates;
    this.ParseTemplate=ParseTemplate;
    this.SetChecked=SetChecked;
    this.SetHiddenColumn=SetHiddenColumn;
    this.UnsetHiddenColumn=UnsetHiddenColumn;
    this.UnsetHiddenColumns=UnsetHiddenColumns;
}

function SetHiddenColumn ( l1x ){
    this.hddCs[l1x]=true;
}

function UnsetHiddenColumn ( l1x ){
    this.hddCs[l1x]=false;
}

function UnsetHiddenColumns (){
    this.hddCs=new Array ();
}

function ParseTemplate ( rowIndex, template ){
    var pattern=new RegExp ('(\\{' + 'ROW' + '\\})');
    while (template.match(pattern)) template=template.replace(pattern, rowIndex);
    for (var columIndex=0; columIndex < this.ColumnsCount(); columIndex++){var pattern=new RegExp ('\\{' + columIndex + '\\}');
    while (template.match(pattern)) template=template.replace(pattern, this.Dsr7[rowIndex][columIndex]);
    }
    return template;
}

function SetColumnTemplate ( l1x, template ){
    this.csTes[l1x]=template;
}

function SetRowsTemplate ( template ){
    this.rsTe=template;
}

function UnsetColumnsTemplates (){
    this.csTes=new Array ();
}

function UnsetRowsTemplate (){
    this.rsTe='';
}

function UnsetTemplates (){
    this.UnsetColumnsTemplates ();
    this.UnsetRowsTemplate ()
}

function SetChecked ( checkboxName, checkedElements ){
    var dT=eval(this.dT);
    var checkboxElements=dT.forms[0].elements[checkboxName];
    if(checkboxElements!=null ){
        var checkboxElementsCount=(checkboxElements.length!='undefined') ? checkboxElements.length : 1;
        if(checkboxElementsCount>1 ){
            for (var i=0; i < checkboxElementsCount; i++){
                for (var j=0; j < checkedElements.length; j++){
                    if (checkboxElements[i].value==checkedElements[j]){
                        checkboxElements[i].checked=true;
                    }
                }
            }
        }
        else{
            for (var ii=0; ii < checkedElements.length; ii++){
                if(checkboxElements.value==checkedElements[ii].toString() ){
                    checkboxElements.checked=true;
                }
            }
        }
    }
}

function fvm590gdjfv (){
    this.hCs=arguments;
}

function fdjklfbxv (){
    this.CsWh1=arguments;
}

function hCsAt (){
    this.hCsAt=arguments;
}

function evbfbdd (){
    this.bCat0=arguments;
}

function vckdd8df (){
    this.Dsr7[this.Dsr7.length]=arguments;
}

function vrgf4gl1 (){
    return this.Dsr7.length;
}

function bvdf4gf (){
    return this.hCs.length;
}

function blkkun (){
    this.Dsr7=new Array ();
}

function SetColumnWidth ( zUlu, l1x, cWW2 ){
    if(this.hddCs[l1x] ) return;
    var cO001=eval(this.dT + '.all.column' + l1x);
    var cO30=eval(this.dT + '.all.columnTitle' + l1x);
    if(!cWW2 || cWW2=='undefined' ){
        var cWW2=this.ColumnWidthTruncation ( zUlu, l1x );
    }
    for (var row=1; row < zUlu.rows.length; row++) {
        var nBel=zUlu.rows(row).cells(l1x).firstChild;
        nBel.style.overflow='hidden';
        nBel.style.width=cWW2;
        nBel.style.fontFamily='Sans-Serif, Tahoma';
        nBel.style.fontSize='11px';
    }
    cO001.style.width=cWW2;cO30.style.overflow='hidden';
    cO30.style.width=cWW2;cO30.style.fontFamily='Sans-Serif, Tahoma';cO30.style.fontSize='11px';
}

function ColumnWidthTruncation ( zUlu, l1x ){
    var cWW2Truncation=0;
    var cellWidth=0;
    var cellsNumber=0;
    for (var row=1; row < zUlu.rows.length; row++) {
        cellWidth=zUlu.rows(row).cells(l1x).firstChild.scrollWidth;
        if(cellWidth>rAMW ) {
            cWW2Truncation+=cellWidth;cellsNumber++;
        }
    }
    if(!cWW2Truncation ){
        return eval(this.dT + '.all.column' + l1x).scrollWidth;
    }
    return Math.max(rAMW, cWW2Truncation/cellsNumber);
}

function fvm409gjskldjr ( oT, dT ){
    if(this.ColumnsCount() ){
        if(!dT ){
            dT=this.dT
        }
        else{
            this.dT=dT;
        }
        if(!oT){
            oT=this.oT;
        }
        else{
            this.oT=oT;
        }
        if(dT!='document' ){
            var usingFrames=true;
        }
        else{
            var usingFrames=false;
        }
        var D4fc='';
        var rrCt=this.RowsCount();
        var ccCt=this.ColumnsCount();
        var vvCsC=ccCt-this.hddCs.length;
        if(this.tA ){
            D4fc+='<DIV align=' + this.tA + ((this.tH) ? ' style="height: ' + this.tH + '"' : '') + '>';
        }
        D4fc+='<SPAN style="' + ((this.tW!=null) ? 'width: ' + this.tW + '; ' : '') + ((this.tH!=null) ? 'height: ' + this.tH + '; ' : '') + 'overflow-x: ' + ((tHS) ? tHS : 'hidden') + '; ' +'overflow-y: ' + ((tVS) ? tVS : 'auto') + '; ' + 'border: ' + cBW + 'px inset">\n';
        D4fc+='<TABLE width=100% height=100% cellSpacing=0 cellPadding=0 id=' + this.TID + ' style="cursor: default; table-layout: fixed" onMouseOver="javascript:selection.empty();" onClick="javascript:selection.empty();" onSelectStart="javascript:selection.empty();">\n';
        D4fc+='<TR height=' + hRH + ' bgcolor=' + tHB + '>\n';

//这里是表头的函数
        for (var col=0; col < ccCt; col++){
            if(!this.hddCs[col] ){
                var llD0=this.hCs[col];if(!llD0.match('<IMG') ) llD0='  ' + llD0 + '  ';
             			D4fc+='<TD id=column' + col + ' style="border-top: #9AC7F0 1px solid; border-left: #9AC7F0 1px solid; border-bottom: #294767 1px solid;' + ((aTRLC || col!=ccCt - 1 || vvCsC==1) ? 'border-right: #294767 1px solid;' : '') + '"' + /*((this.CsWh1[col]) ? ' width=' + this.CsWh1[col] : '') + */'">\n' +'<TABLE width=100% height=100% cellSpacing=0 cellPadding=0 border=0 STYLE="color:#ffffff;Dtable-layout: fixed">\n' +'<TR>\n'+'<TD' + ((this.hCsAt[col]) ? ' align=' + this.hCsAt[col] : '') + '><NOBR id=columnTitle' + col + ' style="text-overflow: ellipsis; overflow: hidden">' + llD0 + '</NOBR></TD>\n';
                if(aTRLC || col!=(/*Last column*/ccCt - 1) ){
                    D4fc+='<TD width=3><IMG src="" width=3 height=100% border=0 style="position: relative; left: -1px" alt="' + sRTD + '" onMouseOver="this.style.cursor=\'E-resize\';" onMouseDown="javascript:' + ((usingFrames) ? 'top.' : '') + 'ResizeColumnOnMouseDownEvent(\'' + this.TID + '\', ' + col + ', \'' + dT + '\')"></TD>\n';
                }
                D4fc+='</TR>\n'+'</TABLE>'+'</TD>\n';
            }
        }
        D4fc+=(aTRLC) ? '<TD style="border-top: buttonhighlight 1px solid; border-left: buttonhighlight 1px solid;' + ' border-bottom: buttonshadow 1px solid;"><NOBR><IMG src="images/spacer.gif" width=10 height=1 border=0></NOBR></TD>\n' : '';
        D4fc+='</TR>\n';


//这里是表格内容的函数
        for (var row=0; row < rrCt; row++){
            D4fc+='<TR bgcolor=#FFFFFF ' + ((this.rsTe) ? this.ParseTemplate (row, this.rsTe) : '') + ((rBH!='') ?' onMouseOver="this.style.background=\'' + rBH + '\'; this.style.color=\'' + rTH + '\';" onMouseOut="this.style.background=\'#FFFFFF\'; this.style.color=\'#000000\';"' : '') + '>\n';
            for (var col=0; col < ccCt; col++){
                if(!this.hddCs[col] ){
                    var llD0=(this.csTes[col]) ? this.ParseTemplate (row, this.csTes[col]) : '  ' + this.Dsr7[row][col] + '  ';
                    D4fc+='<TD' + ((this.bCat0[col]) ? ' align=' + this.bCat0[col] : '') + ' style="border-bottom: #C6C3C6 1px solid;' + ((aTRLC || col!=ccCt - 1 || vvCsC==1) ? 'border-right: #C6C3C6 1px solid;' : '') + '"><NOBR style="text-overflow: ellipsis; overflow: hidden" >' + llD0 + '</NOBR></TD>\n';
                }
            }
            D4fc+=(aTRLC) ? '<TD style="border-bottom: #C6C3C6 1px solid;"><NOBR><IMG src="images/spacer.gif" width=10 height=1 border=0></NOBR></TD>\n' : '';
            D4fc+='</TR>\n';
        }
        for (var row=rrCt; row < this.fRN; row++){
            D4fc+='<TR height=' + bRH + ' bgcolor=#FFFFFF>\n';
            for (var col=0; col < ccCt; col++){
                if(!this.hddCs[col] ){
                    D4fc+='<TD style="border-bottom: #C6C3C6 1px solid;' + ((aTRLC || col!=ccCt - 1 || vvCsC==1) ? 'border-right: #C6C3C6 1px solid;' : '') + '"><NOBR> </NOBR></TD>\n';
                }
            }
            D4fc+=(aTRLC) ? '<TD style="border-bottom: #C6C3C6 1px solid;"><NOBR><IMG src="images/spacer.gif" width=10 height=1 border=0></NOBR></TD>\n' : '';
            D4fc+='</TR>\n';
        }
        D4fc+='</TABLE></SPAN>';
        D4fc+='<SPAN id=reAa' + this.TID + ' style="position: absolute; ' + ((tRABL  ) ? 'border-left: '+ tRABL   + '; ' : '') + ((tRABR ) ? 'border-right: '         + tRABR  + '; ' : '') + ((tRABT   ) ? 'border-top: '+ tRABT    + '; ' : '') + ((tRABB) ? 'border-bottom: '+ tRABB + '; ' : '') + ((tRAB  ) ? 'background-color: '     + tRAB   + '; ' : '') + ((tRAO     ) ? 'filter: alpha(opacity=' + tRAO      + ');' : '') + 'top: 0; left: 0px; width: 0px; height: 0px; z-index: 2; display: none;" onMouseMove="javascript:' + ((usingFrames) ? 'top.' : '') + 'ResizeColumnOnMouseMoveEvent(\'' + this.TID + '\', \'' + dT + '\');" onMouseUp="javascript:' + ((usingFrames) ? 'top.' : '') + 'ResizeColumnOnMouseUpEvent(\'' + this.TID + '\', \'' + dT + '\');"></SPAN>';
        if (this.tA){
            D4fc+='</DIV>';
        }
        var zUlu=eval(dT + '.all.' + this.TID);
        if(zUlu!=null ){
            for (var col=0; col < ccCt; col++){
                if(!this.hddCs[col] ){
                    var cO001=eval(dT + '.all.columnTitle' + col);
                    var zUlu =eval(dT + '.all.' + this.TID);
                    if(dT=='document' ){
                        var bWWW=cBW;
                    }
                    else{
                        var bWWW=top.cBW;
                    }
                    var cWW2=cO001.offsetWidth;this.CsWh1[col]=cWW2;
                }
            }
            var obj=zUlu.parentElement;
            if(obj.parentElement.tagName=="DIV" )obj=obj.parentElement;obj.outerHTML=D4fc;
        }
        else{
            if(!oT ){
                    eval(dT).write(D4fc);
            }
            else{
                eval(dT + '.all.' + oT).style.overflow="";
                eval(dT + '.all.' + oT).innerHTML=D4fc;
            }
        }
        for(var col=0;col<ccCt;col++){
            if(!this.hddCs[col] ){
                if(aTRLC||col<ccCt-1){
                    var cO001=eval(dT + '.all.column' + col);
                    var zUlu=eval(dT + '.all.' + this.TID);
                    this.SetColumnWidth ( zUlu, col, this.CsWh1[col] );
                }
            }
        }
    }
    else{
        alert("Error: Table data is undefined.");
    }
}

function GetGlobalOffset ( obj, side ){
    switch(side){
        case "top":
        case "left":
        var leftOffset=0;
        var topOffset=0;
        while(obj.tagName!='BODY'){
            leftOffset+=obj.offsetLeft;
            topOffset +=obj.offsetTop;
            obj=obj.offsetParent;
        }
        return eval(side + 'Offset');
        default:return 0;
    }
}

function ResizeColumnOnMouseDownEvent ( TID, l1x, dT ){
    var cO001=eval(dT + '.all.column' + l1x);
    var zUlu=eval(dT + '.all.' + TID);
    var reAa=eval(dT + '.all.reAa' + TID);
    if(dT=='document' ){
        rCO=cO001;
        rCI=l1x;
        var tLO5=GetGlobalOffset(zUlu, 'left');
        var tT0O=GetGlobalOffset(zUlu, 'top');
        var bWWW=cBW;
    }
    else{
        top.rCO=cO001;
        top.rCI=l1x;
        var tLO5=top.GetGlobalOffset(zUlu, 'left');
        var tT0O =top.GetGlobalOffset(zUlu, 'top');
        var bWWW=top.cBW;
    }
    var tH=zUlu.parentElement.scrollHeight;
    var tW=zUlu.parentElement.scrollWidth;
    var tableScrollTop =zUlu.parentElement.scrollTop;
    var tableScrollLeft=zUlu.parentElement.scrollLeft;
    var tableSpanHeight=zUlu.parentElement.style.pixelHeight;
    var tableSpanWidth=zUlu.parentElement.style.pixelWidth;
    var cellEncirclement=bWWW*2;
    var cWW2=cO001.scrollWidth+cellEncirclement;
    var hRH=cO001.scrollHeight+cellEncirclement;
    var cLot=cO001.offsetLeft;
    var pixelLeft=tLO5+cLot-tableScrollLeft;
    if(pixelLeft<tLO5){
        var reAaOffset=tableScrollLeft - cLot;
        cWW2 -= reAaOffset - bWWW;
        pixelLeft+=reAaOffset + bWWW;
        reAa.style.borderLeft="";
    }
    else{
        reAa.style.borderLeft=tRABL;
    }
    var ciHT2=(zUlu.parentElement.clientHeight + bWWW) - hRH + tableScrollTop;
    reAa.style.pixelLeft  =pixelLeft;
    reAa.style.pixelTop   =tT0O + hRH - tableScrollTop;
    reAa.style.pixelWidth =cWW2;
    reAa.style.pixelHeight=ciHT2;
    reAa.style.display="inline";
    reAa.setCapture();
}

function ResizeColumnOnMouseMoveEvent ( TID, dT ){
    var zUlu =eval(dT + '.all.' + TID);
    var reAa  =eval(dT + '.all.reAa' + TID);
    if(dT=='document' ){
        var cO001=rCO;
        var tLO5=GetGlobalOffset(zUlu, 'left');
        var tT0O =GetGlobalOffset(zUlu, 'top');
        var aMWW=rAMW;
    }
    else{
        var cO001=top.rCO;
        var tLO5=top.GetGlobalOffset(zUlu, 'left');
        var tT0O =top.GetGlobalOffset(zUlu, 'top');
        var aMWW=top.rAMW;
    }
    var cLot=cO001.offsetLeft;
    var tW=zUlu.parentElement.scrollWidth;
    var tableScrollLeft=zUlu.parentElement.scrollLeft;
    var tableSpanWidth=zUlu.parentElement.style.pixelWidth;
    var reAaRightBorder=zUlu.parentElement.clientWidth + tLO5;
    var cWW2=Math.min(eval(dT.substr(0, dT.length-8) + 'window.event.clientX'), reAaRightBorder) + tableScrollLeft - (tLO5 + cLot);
    var pixelLeft=tLO5 + cLot - tableScrollLeft;
    if(pixelLeft < tLO5 ){
        var reAaOffset=tableScrollLeft - cLot;
        cWW2 -= reAaOffset;
        pixelLeft+=reAaOffset;
    }
    cWW2=Math.max(cWW2, aMWW);
    reAa.style.width=cWW2;
}

function ResizeColumnOnMouseUpEvent ( TID, dT ){
    var zUlu=eval(dT + '.all.' + TID);
    if(dT=='document' ){
        var cO001=rCO;var l1x=rCI;
        var tLO5=GetGlobalOffset(zUlu, 'left');
        var tT0O =GetGlobalOffset(zUlu, 'top');
        var aMWW=rAMW;
    }
    else{
        var cO001=top.rCO;
        var l1x=top.rCI;
        var tLO5=top.GetGlobalOffset(zUlu, 'left');
        var tT0O =top.GetGlobalOffset(zUlu, 'top');
        var aMWW=top.rAMW;
    }
    var cO30=eval(dT + '.all.columnTitle' + l1x);
    var reAa=eval(dT + '.all.reAa' + TID);
    var cLot   =cO001.offsetLeft;
    var cWW2=eval(dT.substr(0, dT.length-8) + 'window.event.clientX') + zUlu.parentElement.scrollLeft - (tLO5 + cLot);
    cWW2=Math.max(cWW2, aMWW);
    reAa.releaseCapture();
    for (row=1; row < zUlu.rows.length; row++) {
        var nBel=zUlu.rows(row).cells(l1x).firstChild;
        nBel.style.overflow='hidden';
        nBel.style.width=cWW2;
    }
    cO001.style.width=cWW2;
    cO30.style.overflow='hidden';
    cO30.style.width=cWW2;
    reAa.style.display="none";
}

var tableHighlightedRowObj = null;
var tableRowsBackgroundColor = '#FFFFFF';
var tableRowsTextColor = '#000000';
var tableRowsHighlightBackgroundColor = 'highlight';
var tableRowsHighlightTextColor = 'highlighttext';

function TableRowHighlight ( rowObject, resetRows, resetPrevOnly ){
    if (resetRows == true || resetRows == undefined ){
        if ( resetPrevOnly == true ){
            if ( tableHighlightedRowObj != null ){
                RowHighlight (tableHighlightedRowObj, false);
            }
        }
        else{
            TableRowsReset(rowObject.parentElement);
        }
    }
    if ( !RowHighlighted(rowObject) ){
        RowHighlight(rowObject, true);
    }
    else{
        RowHighlight (rowObject, false);
    }
    tableHighlightedRowObj = rowObject;
}

function TableRowsReset ( tableObject ){
    for (var i = 1; i < tableObject.rows.length; i++){
        var rowObject = tableObject.rows[i];RowHighlight ( rowObject, false );
    }
}

function RowHighlight ( rowObject, highlightRow ){
    if ( highlightRow == true || highlightRow == undefined ){
        var backgorundColor = tableRowsHighlightBackgroundColor;
        var textColor = tableRowsHighlightTextColor;
    }
    else{
        var backgorundColor = tableRowsBackgroundColor;var textColor = tableRowsTextColor;
    }
    rowObject.style.background = backgorundColor;
    for (var i = 0; i < rowObject.childNodes.length; i++){
        rowObject.childNodes[i].style.color = textColor;
    }
}

function RowHighlighted ( rowObject ){
    return (rowObject.style.background == tableRowsHighlightBackgroundColor) ? true : false;
}

