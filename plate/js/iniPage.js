//��check��ֵд������������ѡ��Ϊ1,��ѡ��Ϊ0
function CheckToHidden(oCheck,oHidden)
{
	oCheck=eval("document.all."+oCheck);
	oHidden=eval("document.all."+oHidden);
	if(typeof(oCheck)!="object"||typeof(oHidden)!="object")
		return;
	if(oCheck.tagName!="INPUT"||oHidden.tagName!="INPUT")
		return;
	oHidden.value=(oCheck.checked)?1:0;
}
//������������ֵ��ʾ��check��1Ϊѡ��,0��ѡ��
function HiddenToCheck(oHidden,oCheck)
{
	var oCheck=eval("document.all."+oCheck);
	var oHidden=eval("document.all."+oHidden);
	if(typeof(oCheck)!="object"||typeof(oHidden)!="object")
		return;
	if(oCheck.tagName!="INPUT"||oHidden.tagName!="INPUT")
		return;
	oCheck.checked=(oHidden.value>0)?true:false;
}
//������������ֵ��ʾ��check��3Ϊѡ��,0��ѡ��
function HiddenToCheck2(oHidden,oCheck)
{
	var oCheck=eval("document.all."+oCheck);
	var oHidden=eval("document.all."+oHidden);
	if(typeof(oCheck)!="object"||typeof(oHidden)!="object")
		return;
	if(oCheck.tagName!="INPUT"||oHidden.tagName!="INPUT")
		return;
	oCheck.checked=(oHidden.value==3)?true:false;
}
//��oRadio��ֵд������������ѡ��Ϊ1,��ѡ��Ϊ0
function RadioToHidden(oRadio1,oRadio2,oHidden)
{
	var oRadio1=eval("document.all."+oRadio1);
	var oRadio2=eval("document.all."+oRadio2);
	var oHidden=eval("document.all."+oHidden);
	if(typeof(oRadio1)!="object"||typeof(oRadio2)!="object"||typeof(oHidden)!="object")
		return;
	if(oRadio1.tagName!="INPUT"||oRadio2.tagName!="INPUT"||oHidden.tagName!="INPUT")
		return;
	oHidden.value=(oRadio1.checked)?1:0;
}
//��oRadio��ֵд������������ѡ��Ϊ1,��ѡ��Ϊ2
function RadioToHidden2(oRadio1,oRadio2,oHidden)
{
	var oRadio1=eval("document.all."+oRadio1);
	var oRadio2=eval("document.all."+oRadio2);
	var oHidden=eval("document.all."+oHidden);
	if(typeof(oRadio1)!="object"||typeof(oRadio2)!="object"||typeof(oHidden)!="object")
		return;
	if(oRadio1.tagName!="INPUT"||oRadio2.tagName!="INPUT"||oHidden.tagName!="INPUT")
		return;
	oHidden.value=(oRadio1.checked)?1:2;
}
//������������ֵ��ʾ��oRadio1��1Ϊѡ��,0��ѡ��
function HiddenToRadio(oHidden,oRadio1,oRadio2)
{
	var oRadio1=eval("document.all."+oRadio1);
	var oRadio2=eval("document.all."+oRadio2);
	var oHidden=eval("document.all."+oHidden);
	if(typeof(oRadio1)!="object"||typeof(oRadio2)!="object"||typeof(oHidden)!="object")
		return;
	if(oRadio1.tagName!="INPUT"||oRadio2.tagName!="INPUT"||oHidden.tagName!="INPUT")
		return;
	oRadio1.checked=(oHidden.value==1)?true:false;
	oRadio2.checked=(oHidden.value==1)?false:true;
}
//��oSelect��ֵд������������ѡ��Ϊ1,��ѡ��Ϊ0
function SelectToHidden(oSelect,oHidden)
{
	var oSelect=eval("document.all."+oSelect);
	var oHidden=eval("document.all."+oHidden);
	if(typeof(oSelect)!="object"||typeof(oHidden)!="object")
		return;
	if(oSelect.tagName!="SELECT"||oHidden.tagName!="INPUT")
		return;
	var item=new Array();
	for(var i=0;i<oSelect.options.length;i++)
	{
		item[i]=oSelect.options.text;
	}
	oHidden.value=item.toString();
}
//������������ֵ��ʾ��oSelect,ֵ�Զ��ŷֿ���
function HiddenToSelect(oHidden,oSelect)
{
	var oSelect=eval("document.all."+oSelect);
	var oHidden=eval("document.all."+oHidden);
	if(typeof(oSelect)!="object"||typeof(oHidden)!="object")
		return;
	if(oSelect.tagName!="SELECT"||oHidden.tagName!="INPUT")
		return;
	var ooption=new Array();
	var item=new Array();
	item=oHidden.value.split(',');
	while(oSelect.options.length!=0)
	{  
       for(var i=0;i<oSelect.options.length;i++)
            oSelect.options.remove(i);
        oSelect.options.length=oSelect.options.length/2;
     }
	 for(var i=0;i<item.length;i++)
	 {
 	 	ooption[i]=document.createElement("option");
		ooption[i].text=item[i];
	    oSelect.add(ooption[i]);
   	 }
}
//���ԡ�|���ֿ����ַ�����ֵ��ʾ��oSelect�С�
function StringToSelect(sString,oSelect)
{
	oSelect=eval("document.all."+oSelect);
	if(typeof(oSelect)!="object"||typeof(sString)!="string")
		return;
	if(oSelect.tagName!="SELECT")
		return;
	var ooption=new Array();
	var item=new Array();
	item=sString.split('|');
	while(oSelect.options.length!=0)
	{  
       for(var i=0;i<oSelect.options.length;i++)
            oSelect.options.remove(i);
        oSelect.options.length=oSelect.options.length/2;
     }
	 for(var i=0;i<item.length;i++)
	 {
 	 	ooption[i]=document.createElement("option");
		ooption[i].text=item[i];
	    oSelect.add(ooption[i]);
   	 }
}
//��caStr��ʾ��oSelectֵ�Զ��ŷֿ���1Ϊѡ��,0��ѡ��
function StrToSelect(caStr,oSelect)
{
	var oSelect=eval("document.all."+oSelect);
	if(typeof(oSelect)!="object"||caStr.length==0)
		return;
	if(oSelect.tagName!="INPUT")
		return;
	var ooption=new Array();
	var item=new Array();
	item=caStr.split(',');
	while(oSelect.options.length!=0)
	{  
       for(var i=0;i<oSelect.options.length;i++)
            oSelect.options.remove(i);
        oSelect.options.length=oSelect.options.length/2;
     }
	 for(var i=0;i<item.length;i++)
	 {
 	 	ooption[i]=document.createElement("option");
		ooption[i].text=item[i];
	    oSelect.add(ooption[i]);
   	 }
}
//string to select include text and value
function StringToSelect(sStrText,sStrValue,oSelect)
{
	var oSelect=eval("document.all."+oSelect);
	if(typeof(oSelect)!="object"||typeof(sStrText)!="string"||typeof(sStrValue)!="string")
		return;
	if(oSelect.tagName!="SELECT")
		return;
	var ooption=new Array();
	var tex=new Array();
	var val=new Array();
	tex=sStrText.split('|');
	val=sStrValue.split('|');
	if(tex.length!=val.length)
	{
		alert("��ʼ���ֶδ��󣡣���");
		return;
	}
	while(oSelect.options.length!=0)
	{  
       for(var i=0;i<oSelect.options.length;i++)
            oSelect.options.remove(i);
        oSelect.options.length=oSelect.options.length/2;
     }
	 for(var i=0;i<val.length;i++)
	 {
 	 	ooption[i]=document.createElement("option");
		ooption[i].text=tex[i];
		ooption[i].value=val[i];
	    oSelect.add(ooption[i]);
   	 }
}
function StringToSelect2(sStrText,sStrValue,oSelect)
{
	if(typeof(oSelect)!="object"||typeof(sStrText)!="string"||typeof(sStrValue)!="string")
		return;
	if(oSelect.tagName!="SELECT")
		return;
	var ooption=new Array();
	var tex=new Array();
	var val=new Array();
	tex=sStrText.split('|');
	val=sStrValue.split('|');
	if(tex.length!=val.length)
	{
		alert("��ʼ���ֶδ��󣡣���");
		return;
	}
	while(oSelect.options.length!=0)
	{  
       for(var i=0;i<oSelect.options.length;i++)
            oSelect.options.remove(i);
        oSelect.options.length=oSelect.options.length/2;
     }
	 for(var i=0;i<val.length;i++)
	 {
 	 	ooption[i]=document.createElement("option");
		ooption[i].text=tex[i];
		ooption[i].value=val[i];
	    oSelect.add(ooption[i]);
   	 }
}
//��õ�ǰ�����ʱ�䣺����ֵ-iFormat=1 'YYYY-MM-DD HH24:MI';iFormat=2 'YYYY-MM-DD'
function now(iFormat)
{
	var year=(new Date()).getFullYear().toString();
	var month=((new Date()).getMonth()+1).toString();
	var day=(new Date()).getDate().toString();
	var hour=(new Date()).getHours().toString();
	var minute=(new Date()).getMinutes().toString();
	month=(month.length==1)?"0"+month:month;
	day=(day.length==1)?"0"+day:day;
	hour=(hour.length==1)?"0"+hour:hour;
	minute=(minute.length==1)?"0"+minute:minute;
	if(iFormat=="YYYY-MM-DD HH24:MI")
		return year+"-"+month+"-"+day+" "+hour+":"+minute;
	else if(iFormat=="YYYY-MM-DD")
		return year+"-"+month+"-"+day;
	else
		return year+"-"+month+"-"+day+" "+hour+":"+minute;
}
function goto(o)
{
	window.open(o,"_self","")
}
function ddd(t)
{
    showModalDialog(t, window, "status:no;center:yes;help:no;minimize:no;maximize:no;border:thick;statusbar:no;dialogWidth:240px;scroll:auto;dialogHeight:400px");
}
function showDia(sWeb,iHei,iWid)
{
    var oDia=showModalDialog(sWeb, window, 'status:no;center:yes;help:no;minimize:no;maximize:no;border:thick;statusbar:no;scroll:auto;dialogHeight:'+iHei+'px;dialogWidth:'+iWid+'px');
	return oDia;
}
function showDia2(sWeb,iHei,iWid)
{
    var oDia=showModelessDialog(sWeb, window, 'status:no;center:yes;help:no;minimize:no;maximize:no;border:thick;statusbar:no;scroll:auto;dialogHeight:'+iHei+'px;dialogWidth:'+iWid+'px');
	return oDia;
}
function prop(o)
{
	newWin=window.open("","");
	tmp=o;
	for(var prop in tmp)
	{
		newWin.document.writeln(prop+"="+tmp[prop]+"<br>");
	}
}
//�������ַ���'YYYY-MM-DD HH24-MI'�滻Ϊ'YYYY-MM-DD HH24:MI'
function dedDate(caDate)
{
	if(caDate.length==0)return;
	caTmp1=caDate.slice(0,-3);

	caTmp2=caDate.slice(-2,caDate.length);
	caTmp=caTmp1+":"+caTmp2;
	return caTmp;
}
//ʱ��Ի���
function timeDia(obj)
{
	show_calendar('form1.'+obj)
}
//ѡ��select,item ��value��ֵ
function selSelect(sel,item)
{
	var i;
	if(typeof(sel)!="object"||sel.tagName!="SELECT")
		return;
	for(i=0;i<sel.options.length;i++)
	{
		if(sel.options[i].value==item)
		{
			sel.options[i].selected=true;
			return;
		}
	}
}
//ֻ�����������ֺ�С����
function inpNum(obj,iInt,iDec)
{
	var str=obj.value;
	var i=0,j=0,z=0;
	for(i=0;i<str.length;i++)
	{
		if(str.charCodeAt(i)<45||str.charCodeAt(i)>58)
		{
			alert("���������ֻ�С���� ��");
			obj.focus();
			return 101;
		}
	}
	if(str.charCodeAt(0)==45)
	{
		j=-1;
		z=2;
	}
	else
	{
		j=0;
		z=1;
	}
	for(i=0;i<str.length;i++,j++)
	{
		if(str.charCodeAt(i)==46)
		{
			if(j>iInt)
			{
				alert("������"+iInt+"λ������"+iDec+"λС�� ��");
				obj.focus();
				return 101;
			}
			break;
		}
		if((j+1)>iInt)
		{
			alert("������"+iInt+"λ������"+iDec+"λС�� ��");
			obj.focus();
			return 101;
		}
	}
 	if(str.length-j-z>iDec)
	{
		alert("������"+iInt+"λ������"+iDec+"λС�� ��");
		obj.focus();
		return 101;
	}
	return 0;
}
//����text�������ַ���λ��
function inpLen(obj,len)
{
	if(typeof(obj)=="undefined"&&obj.tagName!="TEXT")
		return;
	var str=obj.value;
	if(str.length>len)
	{
		alert("�Բ�������ֻ������"+len+"λ�ַ�");
		obj.select();
	}
	
}
//ȥ�ո�
function remBlank(str)
{	
	//ȥǰ��ո�
	for(var i=0;i<str.length;i++)
	{
		if((str.slice(i,i+1)).charCodeAt(0)!=32)
		{
			str=str.slice(i,str.length);
			break;
		}
	}
	if(i==str.length)
	{
		str="";
		return str;
	}
	//ȥ����ո�
	for(var i=str.length-1;i>=0;i--)
	{
		if((str.slice(i,i+1)).charCodeAt(0)!=32)
		{
			str=str.slice(0,i+1);
			break;
		}
	}
	//���ֻ��һ���ո�
	if(str.length==1&&str.charCodeAt(0)==32)
		str="";
	return str;
}
//ת���������ַ���
function codeToStr(strCode,strChar,tbl,col)
{
	var aStr=new Array();
	var aCode=new Array();
	aStr=strChar.split('|');
	aCode=strCode.split('|');

	for(var i=1;i<tbl.rows.length-1;i++)
	{
		code=remBlank(tbl.rows[i].cells[col].innerText);
		for(var j=0;j<aCode.length;j++)
		{
			if(code==aCode[j])
			{
				tbl.rows[i].cells[col].innerText=" "+aStr[j];
				break;
			}
		}
	}
}
//ת���������ַ���
function codeToStr2(strCode,strChar,str)
{
	var aStr=new Array();
	var aCode=new Array();
	aStr=strChar.split('|');
	aCode=strCode.split('|');

	code=remBlank(str);
	for(var j=0;j<aCode.length;j++)
	{
		if(code==aCode[j])
		{
			return aStr[j];
		}
	}

}
function showQuery()
{
	window.parent.showModelessDialog('/Wss/Wss_Pur/win_query.htm', window, 'status:no;center:yes;help:no;minimize:no;maximize:no;border:thick;statusbar:no;scroll:auto;dialogHeight:230px;dialogWidth:380px');
}

//ү�水�س��󽹵㰴����˳���л�
function chaFocus()
{
	var i;
	if(event.keyCode!=13)return;
	if(typeof(document.activeElement)=="undefined")
		eval("document.all."+arguments[0]).focus();
	//��˳��۽�
	for(i=0;i<arguments.length;i++)
	{
		if(arguments[i]==document.activeElement.name)
		{
			i++;
			iCur=(i==arguments.length)?0:i;
			var o=eval("document.all."+arguments[iCur]);
			if(typeof(o)=="object")
			{
				o.focus();
				bFlag='1';
			}
			return;
		}
	}
	if(i==arguments.length)
		eval("document.all."+arguments[0]).focus();
	return 0;
}
//��ӡ
function clickPri()
{
	window.open("/Wss/Wss_Report/win_print.htm","_blank", "width=200 height=170 left=400 top=250");

}
function clickPri2()
{
	window.open("/Wss/Wss_Report/win_print2.htm","_blank", "width=200 height=170 left=400 top=250");

}
//��Դ�ļ�
function openSrc()
{
	if(event.shiftKey==true&&event.ctrlKey==true&&event.altKey==true)
	{
		window.location = "view-source:" + window.location.href ;
	}
}
//��ʽ�����
function formatCurrency(num,cur)
{
	var sSymbol="";
	num=remBlank(num);
	if(num.toString().charCodeAt(0)==45)
	{
		num=num.toString().slice(1,num.length);
		sSymbol="-";
	}
	num = num.toString().replace(/re,/g,'');
	if(isNaN(num))
	{
		num = "0";
//		document.currencyform.input3.value="";
	}
	cents = Math.floor((num*100+0.5)%100); 
	num = Math.floor(num).toString();
	if(cents < 10)
		 cents = "0" + cents; 
	for (var i = 0; i < Math.floor((num.length-(1+i))/3); i++) 
		num = num.substring(0,num.length-(4*i+3))+','+num.substring(num.length-(4*i+3)); 
	return (sSymbol+cur+ num + '.' + cents); 
}
function formatNum(num)
{
	var sSymbol="";
	num=remBlank(num);
	if(num.toString().charCodeAt(0)==45)
	{
		num=num.toString().slice(1,num.length);
		sSymbol="-";
	}
	num = num.toString().replace(/re,/g,'');
	if(isNaN(num))
	{
		num = "0";
//		document.currencyform.input3.value="";
	}
	cents = Math.floor((num*100+0.5)%100); 
	num = Math.floor(num).toString();
	if(cents < 10)
		 cents = "0" + cents; 
	for (var i = 0; i < Math.floor((num.length-(1+i))/3); i++) 
		num = num.substring(0,num.length-(4*i+3))+','+num.substring(num.length-(4*i+3)); 
	if(cents=="00")
		return (sSymbol+num+ '.00'); 
	else	
		return (sSymbol+num + '.' + cents); 
}
function recoverCurrency(str)
{
	if(remBlank(str).length==0)
		return 0;
	return str.replace(/,/g,'');
}
//���볬���� sTbl:Table's name;nCol:Table colum number;sWeb:link webPage;nNum:alter variable number;sDip:display name
//�滻������' &i&'
function insLink(sTbl,nCol,sWeb,nNum,sDip)
{
	var tbl=eval("document.all."+sTbl);
	var td,pattern,val;
	var nLength=tbl.rows.length;
	for(var i=1;i<nLength;i++)
	{
		bFlag=0;
		if(typeof(tbl.rows[i].cells[nCol])=="object")
		{	
			td=tbl.rows[i].cells[nCol];
			sWebAlter=sWeb;
			for(var j=1;j<=parseInt(nNum);j++)
			{
				pattern=new RegExp("&"+j+"&");
				val=eval("td.val"+j);
				if(remBlank(val)=="")
				{
					bFlag=1;
					break;
				}
				sWebAlter=sWebAlter.replace(pattern,val);
			}
			if(bFlag==0)
				td.innerHTML="<a href='javascript:' onclick=creFrame2('"+sWebAlter+"','"+sDip+"',top.document.all.select1)>"+td.innerText+"</a>"+"&nbsp;";
		}
	}
	return 1;
}
function insLink4(sTbl,nCol,sWeb,nNum,sDip)
{
	var tbl=eval("document.all."+sTbl);
	var td,pattern,val;
	var nLength=tbl.rows.length;
	for(var i=1;i<nLength;i++)
	{
		bFlag=0
		if(typeof(tbl.rows[i].cells[nCol])=="object")
		{	
			td=tbl.rows[i].cells[nCol];
			sWebAlter=sWeb;
			for(var j=1;j<=parseInt(nNum);j++)
			{
				pattern=new RegExp("&"+j+"&");
				val=eval("td.val"+j);
				if(remBlank(val)=="")
				if(remBlank(val)=="")
				{
					bFlag=1;
					break;
				}
				sWebAlter=sWebAlter.replace(pattern,val);
			}
			if(bFlag==0)
				td.innerHTML="<a href='' onclick=window.close();window.dialogArguments.parent.parent.creFrame2('"+sWebAlter+"','"+sDip+"',top.document.all.select1);event.returnValue=false;>"+td.innerText+"</a>"+"&nbsp;";
		}
	}
	return 1;
}
//open window
function insLink2(sTbl,nCol,sWeb,nNum,nWidth,nHeight)
{
	var tbl=eval("document.all."+sTbl);
	var td,pattern,val;
	var nLength=tbl.rows.length;
	for(var i=1;i<nLength;i++)
	{
		bFlag=0;
		if(typeof(tbl.rows[i].cells[nCol])=="object")
		{
			td=tbl.rows[i].cells[nCol];
			sWebAlter=sWeb;
			for(var j=1;j<=parseInt(nNum);j++)
			{
				pattern=new RegExp("&"+j+"&");
				val=eval("td.val"+j);
				if(remBlank(val)=="")
				if(remBlank(val)=="")
				{
					bFlag=1;
					break;
				}
				sWebAlter=sWebAlter.replace(pattern,val);
			}
			if(bFlag==0)
				td.innerHTML="<a href='javascript:' onclick=showDia('"+sWebAlter+"',"+nHeight+","+nWidth+")>"+td.innerText+"</a>"+"&nbsp;";
		}
	}
	return 1;
}
//mail to
function insLink3(sTbl,nCol,sWeb,nNum)
{
	var tbl=eval("document.all."+sTbl);
	var td,pattern,val;
	var nLength=tbl.rows.length;
	for(var i=1;i<nLength;i++)
	{
		bFlag=0;
		if(typeof(tbl.rows[i].cells[nCol])=="object")
		{
			td=tbl.rows[i].cells[nCol];
			sWebAlter=sWeb;
			for(var j=1;j<=parseInt(nNum);j++)
			{
				pattern=new RegExp("&"+j+"&");
				val=eval("td.val"+j);
				if(remBlank(val)=="")
				if(remBlank(val)=="")
				{
					bFlag=1;
					break;
				}
				sWebAlter=sWebAlter.replace(pattern,val);
			}
			if(bFlag==0)
				td.innerHTML="<a href='javascript:' onclick=location='mailto:"+sWebAlter+"'>"+td.innerText+"</a>"+"&nbsp;";
		}
	}
	return 1;
}
function insLink5(sTbl,nCol,sWeb,nNum)
{
	var tbl=eval("document.all."+sTbl);
	var td,pattern,val;
	var nLength=tbl.rows.length;
	for(var i=1;i<nLength;i++)
	{
		bFlag=0;
		if(typeof(tbl.rows[i].cells[nCol])=="object")
		{
			td=tbl.rows[i].cells[nCol];
			sWebAlter=sWeb;
			for(var j=1;j<=parseInt(nNum);j++)
			{
				pattern=new RegExp("&"+j+"&");
				val=eval("td.val"+j);
				if(remBlank(val)=="")
				if(remBlank(val)=="")
				{
					bFlag=1;
					break;
				}
				sWebAlter=sWebAlter.replace(pattern,val);
			}
			if(bFlag==0)
				td.innerHTML="<a href='javascript:' onclick=window.close();window.dialogArguments.parent.parent.location='mailto:"+sWebAlter+"';event.returnValue=false;>"+td.innerText+"</a>"+"&nbsp;";
		}
	}
	return 1;
}
function insLink99(sTbl,nCol,sWeb,nNum)
{
	var tbl=eval("document.all."+sTbl);
	var td,pattern,val;
	var nLength=tbl.rows.length;
	for(var i=1;i<nLength;i++)
	{
		bFlag=0;
		if(typeof(tbl.rows[i].cells[nCol])=="object")
		{
			td=tbl.rows[i].cells[nCol];
			sWebAlter=sWeb;
			for(var j=1;j<=parseInt(nNum);j++)
			{
				pattern=new RegExp("&"+j+"&");
				val=eval("td.val"+j);
				if(remBlank(val)=="")
				if(remBlank(val)=="")
				{
					bFlag=1;
					break;
				}
				sWebAlter=sWebAlter.replace(pattern,val);
			}
			if(bFlag==0)
			{
			if(eval("td.val2")=="1")
				td.innerHTML="<a href='javascript:' onclick=creFrame2('"+sWebAlter+"','�����������',top.document.all.select1)>"+td.innerText+"</a>"+"&nbsp;";
			else if(eval("td.val2")=="2")
				td.innerHTML="<a href='javascript:' onclick=creFrame2('"+sWebAlter+"','�����������',top.document.all.select1)>"+td.innerText+"</a>"+"&nbsp;";
			else if(eval("td.val2")=="3")
				td.innerHTML="<a href='javascript:' onclick=creFrame2('"+sWebAlter+"','����ƴ�����',top.document.all.select1)>"+td.innerText+"</a>"+"&nbsp;";
			else if(eval("td.val2")=="4")
				td.innerHTML="<a href='javascript:' onclick=creFrame2('"+sWebAlter+"','����ƴ�����',top.document.all.select1)>"+td.innerText+"</a>"+"&nbsp;";
			else if(eval("td.val2")=="5")
				td.innerHTML="<a href='javascript:' onclick=creFrame2('"+sWebAlter+"','���˽���',top.document.all.select1)>"+td.innerText+"</a>"+"&nbsp;";
			else if(eval("td.val2")=="6")
				td.innerHTML="<a href='javascript:' onclick=creFrame2('"+sWebAlter+"','���˳���',top.document.all.select1)>"+td.innerText+"</a>"+"&nbsp;";
			else if(eval("td.val2")=="7")
				td.innerHTML="<a href='javascript:' onclick=creFrame2('"+sWebAlter+"','��˰��ת��',top.document.all.select1)>"+td.innerText+"</a>"+"&nbsp;";
			else if(eval("td.val2")=="8")
				td.innerHTML="<a href='javascript:' onclick=creFrame2('"+sWebAlter+"','��˰��ת��',top.document.all.select1)>"+td.innerText+"</a>"+"&nbsp;";
			else if(eval("td.val2")=="9")
				td.innerHTML="<a href='javascript:' onclick=creFrame2('"+sWebAlter+"','����',top.document.all.select1)>"+td.innerText+"</a>"+"&nbsp;";
		
			}
		}
	}
	return 1;
}
//��ʾ������ oObj:��Ҫ����ֵ�Ķ���
function showCalculator(oObj)
{
//	if(typeof(oObj)=="object")
	{
		showModalDialog("/Wss/win_calculator.htm",oObj,"scroll:no;center:1;status:0;help:0;edge:raised;dialogHeight:251px;dialogWidth:350px");
	}
}
//��ʼ��Text,button��Ͽؼ�
var bFlag='0';//used to judge the focus src ,from keydown or click
function initTexBut(sText)
{
	var oText=eval("document.all."+sText);
	var oBut=eval("document.all."+sText+"_but");
	oText.onfocus=new Function("eval('document.all."+sText+"_but').style.display='';if(bFlag=='1'){bFlag='0';eval('document.all."+sText+"_but').click();}");
	oText.onblur=new Function("if(document.activeElement.name!='"+sText+"_but')eval('document.all."+sText+"_but').style.display='none'");
	oBut.onblur=new Function("if(document.activeElement.name!='"+sText+"')eval('document.all."+sText+"_but').style.display='none'");

}
//��ʼ��Remark��
function disRemark(sRem)
{
	var oRem=eval("document.all."+sRem);
	oRem.onfocus=new Function("focRemark('"+sRem+"')");
}
function disRemark2(sRem)
{
	var oRem=eval("document.all."+sRem);
	oRem.onclick=new Function("focRemark2('"+sRem+"')");
}
var bFlagRem=0;
function focRemark(sRem)
{
	if(bFlagRem==1)return;
	var oRem=eval("document.all."+sRem);
	if(oRem.value.length!=0)
		document.all.texttt.innerText=oRem.value;
	document.all.remark.style.display="block";
	document.all.remark.flag=sRem;
	document.all.texttt.focus();
}
function focRemarkTD(oRem)
{
	if(bFlagRem==1)return;
	if(remBlank(oRem.innerText).length!=0)
		document.all.texttt.innerText=oRem.innerText;
	document.all.remark.style.display="block";
	document.all.remark.flag=oRem;
	document.all.texttt.focus();
}
function bluRemarkTD(oObj)
{
//	document.all.texttt.onchange="";	
	var oRem=oObj.parentNode.flag;
	oRem.innerText=" "+remBlank(document.all.texttt.innerText);
	document.all.texttt.innerText="";
	document.all.remark.style.display="none";
	bFlagRem=1;
	oRem.focus();
	oRem.onblur=new Function("bFlagRem=0");
	oRem.onkeydown=new Function("return false");
}
function focRemark2(sRem)
{
	if(bFlagRem==1)return;
	var oRem=eval("document.all."+sRem);
	if(oRem.value.length!=0)
	{
		document.all.texttt2.innerText=oRem.value;
		document.all.hiddennn.value=(oRem.value);
	}
	document.all.remark2.style.display="block";
	document.all.remark2.flag=sRem;
	document.all.texttt2.focus();
}
function bluRemark2(oObj)
{

	var oRem=eval("document.all."+oObj.parentNode.flag);
	if(remBlank(document.all.texttt2.innerText)!=remBlank(document.all.hiddennn.value))
	{
		if(typeof(eval("document.all."+oRem.name+"_id"))!="undefined")
			eval("document.all."+oRem.name+"_id").value="";
	}
	oRem.value=document.all.texttt2.innerText;
	document.all.texttt2.innerText="";
	document.all.hiddennn.value="";
	document.all.remark2.style.display="none";
	bFlagRem=1;
	oRem.focus();
	oRem.onblur=new Function("bFlagRem=0");
	oRem.onkeydown=new Function("return false");
}
function bluRemark(oObj)
{
//	document.all.texttt.onchange="";	
	var oRem=eval("document.all."+oObj.parentNode.flag);
	oRem.value=document.all.texttt.innerText;
	document.all.texttt.innerText="";
	document.all.remark.style.display="none";
	bFlagRem=1;
	oRem.focus();
	oRem.onblur=new Function("bFlagRem=0");
	oRem.onkeydown=new Function("return false");
}

function clickESC()
{
 	if(event.keyCode=="27")
	{
		if(typeof(document.all.remark)!="undefined")
		{
			document.all.remark.style.display="none";
		}
		if(typeof(document.all.remark2)!="undefined")
		{
			document.all.remark2.style.display="none";
		}
	}
}
function clickDiaESC()
{
 	if(event.keyCode=="27")
	{
		if(typeof(document.all.remark)!="undefined")
		{
			if(document.all.remark.style.display!="none")
				document.all.remark.style.display="none";
		}
		else 
		{
			self.close();
		}
		
	}
}
//ȫѡ
var bSelCheAll=0;
function selCheAll(sTbl,nCol)
{
	oTbl=eval("document.all."+sTbl);
	oTbl.rows[0].cells[nCol].innerHTML="<input type='checkbox' onclick=cheAll('"+sTbl+"','"+nCol+"')>"
}
function selCheAll3(sTbl,nCol)
{
	oTbl=eval("document.all."+sTbl);
	oTbl.rows[0].cells[nCol].innerHTML="<input type='checkbox' onclick=cheAll2('"+sTbl+"','"+nCol+"')>"
}
function selCheAll2(sTbl,nCol)
{
	oTbl=eval("document.all."+sTbl+"_head");
	oTbl.rows[0].cells[nCol].innerHTML="<input type='checkbox' onclick=cheAll('"+sTbl+"','"+nCol+"')>"
}
function cheAll(sTbl,nCol)
{
	oTbl=eval("document.all."+sTbl);
	if(bSelCheAll==0)
	{
		for(var i=1;i<oTbl.rows.length-1;i++)
		{
			oTbl.rows[i].cells[nCol].childNodes[0].checked=1;
		}
		bSelCheAll=1;
	}
	else if(bSelCheAll==1)
	{
		for(var i=1;i<oTbl.rows.length-1;i++)
		{
			oTbl.rows[i].cells[nCol].childNodes[0].checked=0;
		}
		bSelCheAll=0;
	}
	return true;
}
function cheAll2(sTbl,nCol)
{
	oTbl=eval("document.all."+sTbl);
	if(bSelCheAll==0)
	{
		for(var i=1;i<oTbl.rows.length;i++)
		{
			oTbl.rows[i].cells[nCol].childNodes[0].checked=1;
		}
		bSelCheAll=1;
	}
	else if(bSelCheAll==1)
	{
		for(var i=1;i<oTbl.rows.length;i++)
		{
			oTbl.rows[i].cells[nCol].childNodes[0].checked=0;
		}
		bSelCheAll=0;
	}
	return true;
}
//���ת��д
function chaAmoReg(sAmount)
{
	var sBig="��Ҽ��������½��ƾ�";
	var sResult="";
	var bFlag=0;
	//��������Ƿ�Ϊ�Ϸ����֣�ȡС�������λ���������롣
	var sAmo=new Array();
	var fAmount=parseFloat(sAmount);
	if(isNaN(fAmount))return;
	sAmo=(Math.round(fAmount*100)/100).toString().split(".");
	if(sAmo[0]!="0")
	{
	for(var i=0,j=sAmo[0].length;i<sAmo[0].length;i++,j--)
	{
		if(sAmo[0].charAt(i)!="0")
		{
			sResult+=sBig.charAt(sAmo[0].charAt(i));
			sResult+=getPos(j);
		}
		else
		{
			for(var z=1;i<sAmo[0].length;z++,i++,j--)
			{
				if(j==1)
				{
					sResult+="Ԫ"
					break;
				}
				if(j==5)
				{
					sResult+="��"
					break;
				}
				if(j==9)
				{
					sResult+="��"
					break;
				}
				if(sAmo[0].charAt(i+1)!="0")
				{
					sResult+="��";
					break;
				}
			}
		}
	}
	}
	if(typeof(sAmo[1])!="undefined")
	{
		for(var i=0,j=-1;i<sAmo[1].length;i++,j--)
		{
				sResult+=sBig.charAt(sAmo[1].charAt(i));
				if(sAmo[1].charAt(i)!="0")
					sResult+=getPos(j);
		}
	}
	else
	{
			sResult+="��";
	}
	return sResult;
}
function getPos(nPos)
{
	var sPos="";
	switch(nPos)
	{
		case -1:
			sPos="��";
			break;
		case -2:
			sPos="��";
			break;
		case 1:
			sPos="Ԫ";
			break;
		case 2:
			sPos="ʰ";
			break;
		case 3:
			sPos="��";
			break;
		case 4:
			sPos="Ǫ";
			break;
		case 5:
			sPos="��";
			break;
		case 6:
			sPos="ʰ";
			break;
		case 7:
			sPos="��";
			break;
		case 8:
			sPos="Ǫ";
			break;
		case 9:
			sPos="��";
			break;
		case 10:
			sPos="ʰ";
			break;
		case 11:
			sPos="��";
			break;
		case 12:
			sPos="Ǫ";
			break;
	}
	return sPos;
}
//����ҳ���Ƿ���������,return false δ��������
function cheText()
{
	for(var i=0;i<arguments.length/2;i++)
	{
		if(remBlank(eval("document.all."+arguments[2*i])).value.length==0)
		{
			alert("������"+arguments[2*i+1]);
			eval("document.all."+arguments[2*i]).focus();
			return false;
		}
	}
	return true;
}
//�����������
function swpTblCol(sTbl,nCol1,nCol2)
{
	oTbl=eval("document.all."+sTbl);
//	oTbl.rows[0].cells[nCol1].swapNode(oTbl.rows[0].cells[nCol2]);
	sTmp=oTbl.rows[0].cells[nCol1].innerText;
	oTbl.rows[0].cells[nCol1].innerText=oTbl.rows[0].cells[nCol2].innerText;
	oTbl.rows[0].cells[nCol2].innerText=sTmp;
	sTmp=oTbl.rows[0].cells[nCol1].flag;
	oTbl.rows[0].cells[nCol1].flag=oTbl.rows[0].cells[nCol2].flag;
	oTbl.rows[0].cells[nCol2].flag=sTmp;
	sTmp=oTbl.rows[0].cells[nCol1].width;
	oTbl.rows[0].cells[nCol1].width=oTbl.rows[0].cells[nCol2].width;
	oTbl.rows[0].cells[nCol2].width=sTmp;
	var nLength=oTbl.rows.length-2;
	for(var i=1;i<nLength;i++)
	{
//		if(typeof(oTbl.rows[i].cells[nCol1])!="undefined"&&typeof(oTbl.rows[i].cells[nCol2])!="undefined")
		{
			sTmp=oTbl.rows[i].cells[nCol1].innerText;
			oTbl.rows[i].cells[nCol1].innerText=oTbl.rows[i].cells[nCol2].innerText;
			oTbl.rows[i].cells[nCol2].innerText=sTmp;
		}
	}
}
//���ر��ĳ��
function hidTblCol(sTbl,nCol)
{
	oTbl=eval("document.all."+sTbl);
	for(var i=0;i<oTbl.rows.length;i++)
	{
		if(typeof(oTbl.rows[i].cells[nCol])!="undefined")
			oTbl.rows[i].cells[nCol].style.display='none';
	}
}
//��ʾ���ĳ��
function disTblCol(sTbl,nCol)
{
	oTbl=eval("document.all."+sTbl);
	for(var i=0;i<oTbl.rows.length;i++)
	{
		if(typeof(oTbl.rows[i].cells[nCol])!="undefined")
			oTbl.rows[i].cells[nCol].style.display='block';
	}
}
//on keydown inspect is number
function testNum()
{
	if((event.keyCode<48||event.keyCode>57)&&(event.keyCode<96||event.keyCode>105)&&event.keyCode!=8&&event.keyCode!=46&&(event.keyCode<37||event.keyCode>40)&&event.keyCode!=13)
		event.returnValue=false;
}