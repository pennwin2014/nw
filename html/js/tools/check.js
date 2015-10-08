function f_NwGlobalContext(){
	var RegPhone = /^1\d{10}$|^(0\d{2,3}-?|\(0\d{2,3}\))?[1-9]\d{4,7}(-\d{1,8})?$/;
	var RegMail = /^([a-zA-Z0-9_\.\-])+\@(([a-zA-Z0-9\-])+\.)+([a-zA-Z0-9]{2,4})+$/;
	var RegEnglish = /^[A-Za-z0-9]{4,40}$/;
//校验的方法
	this.isMail = function (mail){
		var filter  = /^([a-zA-Z0-9_\.\-])+\@(([a-zA-Z0-9\-])+\.)+([a-zA-Z0-9]{2,4})+$/;
		if (filter.test(mail)) 
			return true;
		else {
			return false;
		}
	}
	this.isPhone = function(Phone) {
		var filter=/^1\d{10}$|^(0\d{2,3}-?|\(0\d{2,3}\))?[1-9]\d{4,7}(-\d{1,8})?$/;
		if (filter.test(Phone)){
			return true;
		}
		else {
			return false;
		}
	}
	this.isEnglish = function(value){	
		var filter=/[^\u4e00-\u9fa5]/;
		if (filter.test(value)){
			return true;
		}
		else {
			return false;
		}
	}
	
	// 检测QQ
	this.isQQ =function(qq)
	{
		var filter=/^[1-9][0-9]{4,9}$/;
		if (filter.test(qq)){
			return true;
		}
		else {
			return false;
		}
	}
	// 检测网址
	this.isWebUrl =function(url)
	{
			/*
		var filter=(http|ftp|https):\/\/[\w\-_]+(\.[\w\-_]+)+([\w\-\.,@?^=%&amp;:/~\+#]*[\w\-\@?^=%&amp;/~\+#])?;
		if (filter.test(url)){
			return true;
		}
		else {
			return false;
		}
		*/
		return true;
	}
}
















