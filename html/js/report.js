
function GetId(s)
{

	return document.getElementById(s);
}


function objshow(s)
{
    GetId(s).style.display= "";
}

function objhide(s)
{
	GetId(s).style.display="none";
}


function showElement(s)
{

	if(GetId(s).style.display == 'none') {
		objshow(s);
	} else {
      	objhide(s);
  	}

}

function radioVal(r)
{
    var i = 0;
    var str = "";
    var obj = document.getElementsByName(r);
    for(i = 0; i < obj.length; i++)
    {
        if(obj[i].checked)
        {
            str = obj[i].value;
            break;
        }
    }
	return str;
}

function checkVal(chk)
{
    return GetId(chk).checked;
}

String.prototype.trim = function()
{
    return this.replace(/(^[\\s]*)|([\\s]*$)/g, "");
}


function setRadioChecked(r, v)
{
    var obj = document.getElementsByName(r);
    for(i = 0; i < obj.length; i++)
    {
        if(obj[i].value == v)
        {
            obj[i].checked = true;
            break;
        }
    }
}


function OnSelectAll_item(name)
{
	var obj=document.getElementsByName(name);
	var i_idx=0;
	for(i_idx=0;i_idx<obj.length;i_idx++)
	{
		try
		{
			obj[i_idx].checked=true;
		}catch(eee){}
	}
}


function OnSelectOther_item(name)
{
	var obj=document.getElementsByName(name);
	var i_idx=0;
	for(i_idx=0;i_idx<obj.length;i_idx++)
	{
		try
		{
			obj[i_idx].checked=!obj[i_idx].checked;
		}catch(eee){}
	}
}


function IsClickMe(eventId,obj)
  {
  		var inputObj=obj;
  		var t=inputObj.offsetTop;
		var l=inputObj.offsetLeft;

		while(inputObj=inputObj.offsetParent)
		{
			t+=inputObj.offsetTop;
			l+=inputObj.offsetLeft;

		}
		var r=l+Math.abs(obj.clientWidth);//w;//l+w;
		var b=t+Math.abs(obj.clientHeight);//h;

		var x=eventId.x;
		var y=eventId.y;  			
		var sl=(""+window.document.body.scrollLeft).replace(/[^0-9]/g,"");
		var sh=(""+window.document.body.scrollTop).replace(/[^0-9]/g,"");
		x+=Math.abs(sl);
		y+=Math.abs(sh);
		var xx=Math.abs(x);
		var ll=Math.abs(l);
		var yy=Math.abs(y);
		var rr=Math.abs(r);
		var tt=Math.abs(t);
		var bb=Math.abs(b);
		if( (xx>ll) && (xx<rr) && (yy>tt) && (yy<bb))
		{

			return true;
		}else
		{

			return false;
		}
  }

function onSearchDetail(name,id,clearid)
{

    if (checkVal(name))
    {
     objshow(id);
    }
    else 
    {
       objhide(id);
       if(clearid)
       document.getElementById(clearid).value = '';
    }
}

