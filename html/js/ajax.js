 	//����XMLHttpRequest����       
function createXMLHttpRequest() {
	var C=null;
		if(window.XMLHttpRequest) { //Mozilla �����
			C = new XMLHttpRequest();
		}
		else if (window.ActiveXObject) { // IE�����
			try {
				C = new ActiveXObject("Msxml2.XMLHTTP");
			} catch (e) {
				try {
					C = new ActiveXObject("Microsoft.XMLHTTP");
				} catch (e) {}
			}
		}
		return C;
	}
	//�����ַ���  ins-�����ַ�����   names-��ʼ�ַ��� namee -�����ַ��� ����ֵ ��names ��namee ֮����ַ��� �����ڽ����򵥵�XML
	function getxmlbyname(ins,names,namee){
	var start;
	var end;
	var ret='';

	start=ins.indexOf(names);
	end=ins.indexOf(namee);
	
	if(start!=-1&&end!=-1){
		ret=ins.substr(start+names.length,end-start-names.length);
	}
	return ret;	
}

