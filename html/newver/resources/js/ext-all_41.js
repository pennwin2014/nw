/*
Ext JS 4.1 - JavaScript Library
Copyright (c) 2006-2012, Sencha Inc.
All rights reserved.
licensing@sencha.com

http://www.sencha.com/license

Open Source License
------------------------------------------------------------------------------------------
This version of Ext JS is licensed under the terms of the Open Source GPL 3.0 license. 

http://www.gnu.org/licenses/gpl.html

There are several FLOSS exceptions available for use with this release for
open source applications that are distributed under a license other than GPL.

* Open Source License Exception for Applications

  http://www.sencha.com/products/floss-exception.php

* Open Source License Exception for Development

  http://www.sencha.com/products/ux-exception.php


Alternate Licensing
------------------------------------------------------------------------------------------
Commercial and OEM Licenses are available for an alternate download of Ext JS.
This is the appropriate option if you are creating proprietary applications and you are 
not prepared to distribute and share the source code of your application under the 
GPL v3 license. Please visit http://www.sencha.com/license for more details.

--

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT OF THIRD-PARTY INTELLECTUAL PROPERTY RIGHTS.  See the GNU General Public License for more details.
*/
ndexOf(" ",e)+1;if(e===0){break}}return null},clear:function(a,b){if(this.get(a)){b=b||"/";document.cookie=a+"=; expires=Thu, 01-Jan-70 00:00:01 GMT; path="+b}},getCookieVal:function(b){var a=document.cookie.indexOf(";",b);if(a==-1){a=document.cookie.length}return unescape(document.cookie.substring(b,a))}});Ext.define("Ext.util.Grouper",{extend:"Ext.util.Sorter",isGrouper:true,getGroupString:function(a){return a.get(this.property)}});Ext.define("Ext.util.History",{singleton:true,alternateClassName:"Ext.History",mixins:{observable:"Ext.util.Observable"},useTopWindow:true,fieldId:Ext.baseCSSPrefix+"history-field",iframeId:Ext.baseCSSPrefix+"history-frame",constructor:function(){var a=this;a.oldIEMode=Ext.isIE6||Ext.isIE7||!Ext.isStrict&&Ext.isIE8;a.iframe=null;a.hiddenField=null;a.ready=false;a.currentToken=null;a.mixins.observable.constructor.call(a)},getHash:function(){var a=window.location.href,b=a.indexOf("#");return b>=0?a.substr(b+1):null},setHash:function(d){var a=this,c=a.useTopWindow?window.top:window;try{c.location.hash=d}catch(b){}},doSave:function(){this.hiddenField.value=this.currentToken},handleStateChange:function(a){this.currentToken=a;this.fireEvent("change",a)},updateIFrame:function(b){var a='<html><body><div id="state">'+Ext.util.Format.htmlEncode(b)+"</div></body></html>",d;try{d=this.iframe.contentWindow.document;d.open();d.write(a);d.close();return true}catch(c){return false}},checkIFrame:function(){var d=this,b=d.iframe.contentWindow,e,c,a,g;if(!b||!b.document){Ext.Function.defer(this.checkIFrame,10,this);return}e=b.document;c=e.getElementById("state");a=c?c.innerText:null;g=d.getHash();Ext.TaskManager.start({run:function(){var k=b.document,j=k.getElementById("state"),h=j?j.innerText:null,i=d.getHash();if(h!==a){a=h;d.handleStateChange(h);d.setHash(h);g=h;d.doSave()}else{if(i!==g){g=i;d.updateIFrame(i)}}},interval:50,scope:d});d.ready=true;d.fireEvent("ready",d)},startUp:function(){var a=this,b;a.currentToken=a.hiddenField.value||this.getHash();if(a.oldIEMode){a.checkIFrame()}else{b=a.getHash();Ext.TaskManager.start({run:function(){var c=a.getHash();if(c!==b){b=c;a.handleStateChange(b);a.doSave()}},interval:50,scope:a});a.ready=true;a.fireEvent("ready",a)}},init:function(d,b){var c=this,a=Ext.DomHelper;if(c.ready){Ext.callback(d,b,[c]);return}if(!Ext.isReady){Ext.onReady(function(){c.init(d,b)});return}c.hiddenField=Ext.getDom(c.fieldId);if(!c.hiddenField){c.hiddenField=Ext.getBody().createChild({id:Ext.id(),tag:"form",cls:Ext.baseCSSPrefix+"hide-display",children:[{tag:"input",type:"hidden",id:c.fieldId}]},false,true).firstChild}if(c.oldIEMode){c.iframe=Ext.getDom(c.iframeId);if(!c.iframe){c.iframe=a.append(c.hiddenField.parentNode,{tag:"iframe",id:c.iframeId,src:Ext.SSL_SECURE_URL})}}c.addEvents("ready","change");if(d){c.on("ready",d,b,{single:true})}c.startUp()},add:function(a,c){var b=this;if(c!==false){if(b.getToken()===a){return true}}if(b.oldIEMode){return b.updateIFrame(a)}else{b.setHash(a);return true}},back:function(){window.history.go(-1)},forward:function(){window.history.go(1)},getToken:function(){return this.ready?this.currentToken:this.getHash()}});Ext.define("Ext.util.Point",{extend:"Ext.util.Region",statics:{fromEvent:function(a){a=(a.changedTouches&&a.changedTouches.length>0)?a.changedTouches[0]:a;return new this(a.pageX,a.pageY)}},constructor:function(a,b){this.callParent([b,a,b,a])},toString:function(){return"Point["+this.x+","+this.y+"]"},equals:function(a){return(this.x==a.x&&this.y==a.y)},isWithin:function(b,a){if(!Ext.isObject(a)){a={x:a,y:a}}return(this.x<=b.x+a.x&&this.x>=b.x-a.x&&this.y<=b.y+a.y&&this.y>=b.y-a.y)},roundedEquals:function(a){return(Math.round(this.x)==Math.round(a.x)&&Math.round(this.y)==Math.round(a.y))}},function(){this.prototype.translate=Ext.util.Region.prototype.translateBy});
	
 
	
	Ext.define("Ext.view.TableChunker",{singleton:true,requires:["Ext.XTemplate"],metaTableTpl:["{%if (this.openTableWrap)out.push(this.openTableWrap())%}",'<table class="'+Ext.baseCSSPrefix+"grid-table "+Ext.baseCSSPrefix+'grid-table-resizer" border="0" cellspacing="0" cellpadding="0" {[this.embedFullWidth(values)]}>',"<tbody>",'<tr class="'+Ext.baseCSSPrefix+'grid-header-row">','<tpl for="columns">','<th class="'+Ext.baseCSSPrefix+'grid-col-resizer-{id}" style="width: {width}px; height: 0px;"></th>',"</tpl>","</tr>","{[this.openRows()]}","{row}",'<tpl for="features">',"{[this.embedFeature(values, parent, xindex, xcount)]}","</tpl>","{[this.closeRows()]}","</tbody>","</table>","{%if (this.closeTableWrap)out.push(this.closeTableWrap())%}"],constructor:function(){Ext.XTemplate.prototype.recurse=function(b,a){return this.apply(a?b[a]:b)}},embedFeature:function(b,d,a,e){var c="";if(!b.disabled){c=b.getFeatureTpl(b,d,a,e)}return c},embedFullWidth:function(b){var a='style="width:{fullWidth}px;';if(!b.rowCount){a+="height:1px;"}return a+'"'},openRows:function(){return'<tpl for="rows">'},closeRows:function(){return"</tpl>"},metaRowTpl:['<tr class="'+Ext.baseCSSPrefix+'grid-row {[this.embedRowCls()]}" {[this.embedRowAttr()]}>','<tpl for="columns">','<td class="{cls} '+Ext.baseCSSPrefix+"grid-cell "+Ext.baseCSSPrefix+'grid-cell-{columnId} {{id}-modified} {{id}-tdCls} {[this.firstOrLastCls(xindex, xcount)]}" {{id}-tdAttr}>','<div {unselectableAttr} class="'+Ext.baseCSSPrefix+'grid-cell-inner {unselectableCls}" style="text-align: {align}; {{id}-style};">{{id}}</div>',"</td>","</tpl>","</tr>"],firstOrLastCls:function(a,b){if(a===1){return Ext.view.Table.prototype.firstCls}else{if(a===b){return Ext.view.Table.prototype.lastCls}}},embedRowCls:function(){return"{rowCls}"},embedRowAttr:function(){return"{rowAttr}"},openTableWrap:undefined,closeTableWrap:undefined,getTableTpl:function(k,b){var j,h={openRows:this.openRows,closeRows:this.closeRows,embedFeature:this.embedFeature,embedFullWidth:this.embedFullWidth,openTableWrap:this.openTableWrap,closeTableWrap:this.closeTableWrap},g={},c=k.features||[],m=c.length,e=0,l={embedRowCls:this.embedRowCls,embedRowAttr:this.embedRowAttr,firstOrLastCls:this.firstOrLastCls,unselectableAttr:k.enableTextSelection?"":'unselectable=""',unselectableCls:k.enableTextSelection?"":Ext.baseCSSPrefix+""},d=Array.prototype.slice.call(this.metaRowTpl,0),a;for(;e<m;e++){if(!c[e].disabled){c[e].mutateMetaRowTpl(d);Ext.apply(l,c[e].getMetaRowTplFragments());Ext.apply(g,c[e].getFragmentTpl());Ext.apply(h,c[e].getTableFragments())}}d=new Ext.XTemplate(d.join(""),l);k.row=d.applyTemplate(k);a=new Ext.XTemplate(this.metaTableTpl.join(""),h);j=a.applyTemplate(k);if(!b){j=new Ext.XTemplate(j,g)}return j}});Ext._endTime=new Date().getTime();if(Ext._beforereadyhandler){Ext._beforereadyhandler()};
	
	
	
  




	