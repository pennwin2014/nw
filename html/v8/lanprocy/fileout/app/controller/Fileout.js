
Ext.define('Fileout.controller.Fileout', {
  extend: 'Ext.app.Controller',  
  stores: ['List','SoftCombo','ComCombo','SmtpGrid','SenderGrid','ReceiverGrid','SoftGrid','FtpGrid','UsbCombo'],
	models: ['List','Combo','FieldSet'],
  
  views: [
  	'list.FileoutList','tab.FileoutMain',
  	'common.GroupComTree','common.UsbCombo','common.TypeFieldset','common.SmtpSet','common.SenderSet','common.ReceiverSet','common.FileSet','common.FtpSet','common.AddWin','common.AccountSet','common.ChatSoftSet','common.CloudSoftSet','common.FtpSoftSet','common.HttpSoftSet','common.SmtpSoftSet','common.HttpsSoftSet','common.ServerIpSet','common.SoftAddWin','common.SoftAddWin','common.IpAddWin','common.OtherSoftSet','common.FreesoftSet','common.BlocksoftSet','common.FreeAddressSet','common.BlockAddressSet',
  	'tab.Advance','tab.Http','tab.Smtp','tab.Ftp','tab.Cloud','tab.Upan','tab.Share','tab.Chat','tab.Https','tab.Other','tab.Freeblock'
  ],
  
  init: function() {
  	
		this.control({
		  'fileoutlist':{
		    select: this.showTab,
		    render: this.showRender
		  },
		  'button[action=edit]':{
		    click: this.winEdit
		  },
		  'button[action=delete]':{
		    click: this.winDelete
		  },
		  'button[action=add]':{
		    click: this.winAdd
		  },
		  'button[action=mod]':{
		    click: this.winMod
		  },
		  'button[action=del]':{
		    click: this.winDel
		  },
		  'button[action=delall]':{
		    click: this.winDelall
		  },
		  'button[action=addvalue]':{
		    click: this.saveValue
		  },
		  'typefieldset combo':{
		    select: this.selectCombo,
		    render: this.renderCombo
		  },
		  'softaddwin combo':{
		    beforequery: this.comboQuery
  		},
  		
  		'button[action=change]':{
				click: this.changeProcy
			}, 
			'button[action=insert]':{
				click: this.insertProcy
			},  	
  		'button[action=remove]':{
				click: this.removeProcy
			},  		
  		'button[action=removeall]':{
				click: this.removeAllProcy
			},
		  
		  //���ŵ���
		  'groupcomtree button[action=save]':{
		    click: this.saveGroupCom 
		  },
		  
		  //���Ա�ǩ
		  'fileouttabs':{
		    tabchange: this.changeTab
		  }
		});
  },
  
  /***********************************
  *         �ļ��ⷢ�༭
  ***********************************/
  winEdit: function(btn){
    var typeName = btn.up('typefieldset').mark;
    var field1 = "";
    var title1 = "";
 
    switch(typeName){
      case 7:
        field1 = "��չ������";
        title1 = "�ļ���չ����������";
        break;
      case 4:
        field1 = "�ʼ���ַ����";
        title1 = "�ʼ���ַ��������";
        break;
      case 5:
        field1 = "�����˵�ַ����";
        title1 = "������ַ��������";
        break;
      case 6:
        field1 = "�ռ��˵�ַ����";
        title1 = "�ռ���ַ��������";
        break;
      case 11:
        field1 = "FTP����������";
        title1 = "FTP��������������";
        break;
      case 13:
        field1 = "�����˺����";
        title1 = "�����˺��������";
        break;
      case 3:
      case 8:
      case 10:
      case 12:
      case 14:
      case 16:
      case 15:
      case 32:
      case 34:
        field1 = "�������";
        title1 = "�����������";
        break;
      case 31:
      case 33:
      case 35:
        field1 = "��������ַ���";
        title1 = "��������ַ����";
        break;
    }
    
    Ext.create('Fileout.view.common.AddWin',{
      title: title1,
      field1: field1,
      button1: '����',
      value1: typeName,
      value3: btn,
      deal: 'insert'
    }).showAt(300,100);
  },
  winAdd: function(btn){
  
    var typeset = btn.up('typefieldset');
    var typeName = typeset.mark;
    var comboValue = typeset.down('grid').down('combo').getRawValue();
    var field1 = "";
    var title1 = "";

    if(comboValue==''){
    	alert('����ѡȡ���ӵ�����');
    	return;
    }
    switch(typeName){
      case 7:
        field1 = "��չ��";
        title1 = "�ļ���չ������";
        break;
      case 4:
        field1 = "�ʼ�������ַ";
        title1 = "�ʼ�������ַ����";
        break;
      case 5:
        field1 = "�����˵�ַ";
        title1 = "������ַ����";
        break;
      case 6:
        field1 = "�ռ��˵�ַ";
        title1 = "�ռ���ַ����";
        break;
      case 11:
        field1 = "FTP������";
        title1 = "FTP����������";
        break;
      case 13:
        field1 = "�����˺�";
        title1 = "�����˺�����";
        break;
      case 32:
      case 34:
        field1 = "������";
        title1 = "������������";
        break;
      case 33:
      case 35:
        field1 = "������IP";
        title1 = "������IP����";
        break;  
      case 3:
      case 8:
      case 10:
      case 12:
      case 14:
      case 16:
      case 15:
        Ext.create('Fileout.view.common.SoftAddWin',{
          value1: typeName,
          value2: comboValue,
          value3: btn,
          deal:  'insert'
        }).showAt(300,100);
        return;
      case 31:

        Ext.create('Fileout.view.common.IpAddWin',{
          value1: typeName,
          value2: comboValue,
          value3: btn,
          deal:  'insert'
        }).showAt(300,100);
        return;
    }
    
    Ext.create('Fileout.view.common.AddWin',{
      title: title1,
      field1: field1,
      value1: typeName,
      button1: '����',
      value2: comboValue,
      value3: btn,
      deal: 'insert'
    }).showAt(300,100);
  },
  winMod: function(btn){
    var typeset = btn.up('typefieldset');
    var grid = typeset.down('grid');
    var selection = grid.getSelectionModel().getLastSelected();
    if(selection){
      var typeName = typeset.mark;
      var comboValue = grid.down('combo').getRawValue();
      var field1 = "";
      var title1 = "";
      
      switch(typeName){
        case 7:
          field1 = "��չ��";
          title1 = "�ļ���չ���޸�";
          break;
        case 4:
          field1 = "�ʼ�������ַ";
          title1 = "�ʼ�������ַ�޸�";
          break;
        case 5:
          field1 = "�����˵�ַ";
          title1 = "������ַ�޸�";
          break;
        case 6:
          field1 = "�ռ��˵�ַ";
          title1 = "�ռ���ַ�޸�";
          break;
        case 11:
          field1 = "FTP������";
          title1 = "FTP�������޸�";
          break;
        case 'chat':
          field1 = "�����˺�";
          title1 = "�����˺��޸�";
          break;
      }
      var win = Ext.create('Fileout.view.common.AddWin',{
        title: title1,
        field1: field1,
        value1: typeName,
        value2: comboValue,
        button1: '�޸�',
        value3: btn,
        deal: 'update',
        recno: selection.get('recno')
      });
      win.down('textfield').setValue(selection.get('name'));
      win.showAt(300,100);
    }else{
      alert("��ѡ��Ҫ�޸ĵ���Ŀ");  
    }
    
  },
  winDel: function(btn){
    var typeset = btn.up('typefieldset');
    var grid = typeset.down('grid');
    var selection = grid.getSelectionModel().getLastSelected();
    if(selection){
      Ext.MessageBox.confirm('����', 'ȷ��Ҫɾ�����', function(btn){
        if (btn == 'yes'){
          Ext.Ajax.request({
      	    url: '/pronline/Msg',
      	    params: {
      	    	FunName: "lan_dealParams",
      	    	deal: 'delete',
      	    	recno:selection.get('recno')
      	    },
      	    success: function(response){
      	    	var text = Ext.decode(response.responseText);
      	    	if(text.value=='failure'){
      	    	 	alert("ɾ��ʧ�ܣ�");
      	    	}else{
      	    	  grid.getStore().load({params:{paramtype:typeset.mark,keyname:grid.down('combo').getRawValue()}});
      	      }
      	    }
      		});
        }
      }).setPosition(300,50);
      
    }else{
      alert("��ѡ��Ҫɾ������Ŀ");  
    }
  },
    winDelete: function(btn){
    var typeset = btn.up('typefieldset');
 
    var coma=typeset.down('grid').down('combo');
   
    var delItem=coma.getRawValue();
   
    if(delItem!=''){
      Ext.MessageBox.confirm('����', 'ȷ��Ҫɾ�����', function(btn){
        if (btn == 'yes'){
          Ext.Ajax.request({
      	    url: '/pronline/Msg',
      	    params: {
      	    	FunName: "lan_dealParams",
      	    	deal: 'deletetype',
      	    	keyname:delItem,
      	    	paramtype:typeset.mark
      	    },
      	    success: function(response){
      	    	var text = Ext.decode(response.responseText);
      	    	if(text.value=='failure'){
      	    	 	alert("ɾ��ʧ�ܣ�");
      	    	}else{
      //	    	  typeset.down('grid').getStore().load({params:{paramtype:typeset.mark}});
      	    	 
      	    	  coma.getStore().load({params:{paramtype:typeset.mark}});
      	    	  alert("�Ѿ��ɹ�ɾ��");
      	    	  delItem=coma.getRawValue();      	    	
      	    	  typeset.down('grid').getStore().load({params:{paramtype:typeset.mark,keyname:delItem}});

      	      }
      	    }
      		});
        }
      }).setPosition(300,50);
      
    }else{
      alert("��ѡ��Ҫɾ�����ļ�����");  
    }
  },
  
  winDelall: function(btn){
    var store = btn.up('typefieldset').down('grid').getStore();
    
    var i = 0;
    var recnos = "";
    store.each(function(rec){
      if(i>0){
        recnos += ',';
      }
      recnos += rec.get('recno');
      i++;
    });
    
    Ext.MessageBox.confirm('����', 'ȷ��Ҫɾ��ȫ���', function(btn){
      if (btn == 'yes'){
        Ext.Ajax.request({
    	    url: '/pronline/Msg',
    	    params: {
    	    	FunName: "lan_dealParams",
    	    	deal: 'delall',
    	    	recno: recnos
    	    },
    	    success: function(response){
    	    	var text = Ext.decode(response.responseText);
    	    	if(text.value=='failure'){
    	    	 	alert("ɾ��ʧ�ܣ�");
    	    	}else{
  	    	    store.removeAll();
  	    	  }
    	    }
    		});
      }
    }).setPosition(300,50);
  },
  saveValue: function(btn){
    var win=btn.up('window');
    
    var content = win.down('combo')?win.down('combo').getRawValue():win.down('textfield').getValue();
 //   alert(content);
  
    Ext.Ajax.request({
	    url: '/pronline/Msg',
	    params: {
	    	FunName: "lan_dealParams",
	    	deal: win.deal,
	    	paramtype:win.value1,
	    	keyname:win.value2?win.value2:content,
	    	keyvalue:win.value2?content:"",
	    	recno:win.recno
	    },
	    success: function(response){
	    	var text = Ext.decode(response.responseText); 
	    	
	      //alert(text.value);
	    	if(text.value=='failure'){
	    	 	alert("�������Ѿ����ڣ�");
	    	}else{
	    		
	    	  if(!win.value2){
	    //	   var store = Ext.ComponentQuery.query('common.TypeFieldset')[0].getStore();
	    	//    store.load();
	  
	   	    win.value3.up('typefieldset').down('grid').down('combo').getStore().load({params:{paramtype:win.value1}});
	  	    	
	    	  }else{
	//    	    win.value3.up('typefieldset').down('grid').getStore().load({params:{paramtype:win.value1,keyname:win.value2}});
	    	  }
	    	 
	    	 //  typeset.down('grid').getStore().load({params:{paramtype:typeset.mark,keyname:delItem}});
	    	     var delItem=win.value3.up('typefieldset').down('grid').down('combo').getRawValue();     
	  
	//    alert(win.value2);
	    if(!win.value2){
	    	 alert("���ӳɹ���");
	 	      win.value3.up('typefieldset').down('grid').getStore().load({params:{paramtype:win.value1,keyname:delItem}});
	 	    }
	 	    else{
	 	    	 win.value3.up('typefieldset').down('grid').getStore().load({params:{paramtype:win.value1,keyname:delItem}});
	 	    }
	    		win.close();
	   // 		 alert("���ӳɹ���");
	    		
	    	}
	    
	    }
		});
   // alert(win.title+",,,"+win.field1+",,,"+win.value1+",,,,"+win.deal);
  },
  selectCombo: function(combo,records,eOpts){
  	
    if(records&&records.length>0){
      combo.up('grid').getStore().load({params:{paramtype:combo.up('typefieldset').mark,keyname:records[0].get('text')}});
    }
  },
  renderCombo: function(combo,eOpts){
    var store = combo.getStore();
    store.load({
      params:{paramtype:combo.up('typefieldset').mark},
      callback:function(records, operation, success){
        if(records[0]){
          combo.select(1);
          combo.up('grid').getStore().load({params:{paramtype:combo.up('typefieldset').mark,keyname:records[0].get('text')}});
        }
      }
    });
  },
  comboQuery: function(queryEvent,eOpts){
    var combo = queryEvent.combo;
    combo.getStore().load({params:{paramtype:30,keyname:combo.up('window').value1}});
  },
  
  /***********************************
  *         �ļ��ⷢ�����б�
  ***********************************/
  //��ʼ�������б�
  showRender: function(){
    Fileout.controller.Fileout.loadFileoutListStore(parent.compid,parent.groupid);
    Fileout.controller.Fileout.setTitle(parent.dxtitle);
  },
  //��ʾ��Ӧ���Ա�ǩ
  showTab: function(thisRM,record,index,eOpts){
  	  var procytype = "";
  	   var arr = new Array();
  	  var form= Ext.ComponentQuery.query('fileouttabs')[0].getActiveTab().getForm();
      var formValues = Ext.ComponentQuery.query('fileouttabs')[0].getActiveTab().getForm().getValues();
      procytype=formValues.type;
      var contrcode=parseInt(record.get('controlcode'));
  //   form.loadRecord(record);
     if(procytype=='advance'){
     	if(contrcode&(0X00000001)){
     		arr.push('1');
     	}
      if(contrcode&(0X00000002)){
     		arr.push('2');
     	}
     	 if(contrcode&(0X00000004)){
     		arr.push('4');
     	}
     	if(contrcode&(0X00000010)){
     		arr.push('16');
     	}
     	if(contrcode&(0X00000020)){
     		arr.push('32');
     	}
     	if(contrcode&(0X00000040)){
     		arr.push('64');
     	}
     	if(contrcode&(0X00000080)){
     		arr.push('128');
     	}
     	 if(contrcode&(0X00000100)){
     		arr.push('256');
     	}
 
  //   arr=arr_t.split(',');
  
    	form.findField('forbidden').setValue({forbidden:arr});
  
    }
    else if(procytype=='http'){
    	if(contrcode&(0X00000001)){
     		arr.push('1');
     	}
      if(contrcode&(0X00000002)){
     		arr.push('2');
     	}    
     	form.findField('mode2').setValue({mode2:arr});
     	var hostmail=record.get('hostmail');
     	var httpname=record.get('httpname');
     	var fileexp=record.get('fileexp');
      var filesize=record.get('filesize');
      var fileexpmode=record.get('fileexpmode');
      var hostmailmode=record.get('hostmailmode');
      var httpmode=record.get('httpmode');
     
     	form.findField('mailcombo').setValue(hostmail);
     	form.findField('filecombo').setValue(fileexp);
     	form.findField('softcombo').setValue(httpname);
	    form.findField('filelimit').setValue(filesize);
     	form.findField('mode2').setValue({mode2:arr});
     	form.findField('mode').setValue({mode:fileexpmode});
     	form.findField('mode_smtp').setValue({mode_smtp:hostmailmode});
     	form.findField('mode_soft').setValue({mode_soft:httpmode});
   
   //  	form.findField('mode2').setValue({mode2:arr});
    }
    else if(procytype=='smtp'){
    	if(contrcode&(0X00000001)){
     		arr.push('1');
     	}
      if(contrcode&(0X00000002)){
     		arr.push('2');
     	}    
     	form.findField('mode2').setValue({mode2:arr});
     	var hostmail=record.get('hostmail');
    
     	var smtpname=record.get('smtpname');
     
     	var fileexp=record.get('fileexp');
      var filesize=record.get('filesize');
   
      var fileexpmode=record.get('fileexpmode');
      var hostmailmode=record.get('hostmailmode');
      var smtpmode=record.get('smtpmode');
      
      var mailsendmode=record.get('mailsendmode');
      var mailsend=record.get('mailsend');
      var mailrevmode=record.get('mailrevmode');
      var mailrev=record.get('mailrev');
      form.findField('filecombo').setValue(fileexp);      
     	form.findField('mailcombo').setValue(hostmail);
     	form.findField('sendercombo').setValue(mailsend);      
     	form.findField('receivercombo').setValue(mailrev);
     	
     	form.findField('softcombo').setValue(smtpname);
	    form.findField('filelimit').setValue(filesize);
     	form.findField('mode2').setValue({mode2:arr});
     	form.findField('mode').setValue({mode:fileexpmode});
     	form.findField('mode_smtp').setValue({mode_smtp:hostmailmode});
     	form.findField('mode_smtpsoft').setValue({mode_smtpsoft:smtpmode});
      form.findField('mode_sender').setValue({mode_sender:mailsendmode});
      form.findField('mode_receiver').setValue({mode_receiver:mailrevmode});
   
   //  	form.findField('mode2').setValue({mode2:arr});
    }
    else if(procytype=='ftp'){
    	if(contrcode&(0X00000001)){
     		arr.push('1');
     	}
      if(contrcode&(0X00000002)){
     		arr.push('2');
     	}    
     	form.findField('mode2').setValue({mode2:arr});
     	var ftpname=record.get('ftpname');
     	var ftpserver=record.get('ftpserver');
     	var fileexp=record.get('fileexp');
      var filesize=record.get('filesize');
      var fileexpmode=record.get('fileexpmode');
      var ftpmode=record.get('ftpmode');
      var ftpservermode=record.get('ftpservermode');
    
     	form.findField('ftpcombo').setValue(ftpname);
     	form.findField('filecombo').setValue(fileexp);
     	form.findField('ftpsoftcombo').setValue(ftpserver);
	    form.findField('filelimit').setValue(filesize);
     	form.findField('mode2').setValue({mode2:arr});
     	form.findField('mode').setValue({mode:fileexpmode});
     	form.findField('mode_ftp').setValue({mode_ftp:ftpmode});
     	form.findField('mode_ftpsoft').setValue({mode_ftpsoft:ftpservermode});
   
   //  	form.findField('mode2').setValue({mode2:arr});
    }
    else if(procytype=='chat'){
    	if(contrcode&(0X00000001)){
     		arr.push('1');
     	}
      if(contrcode&(0X00000002)){
     		arr.push('2');
     	}    
     	form.findField('mode2').setValue({mode2:arr});
     	var chataccount=record.get('chataccount');
     	var chataccountmode=record.get('chataccountmode');
     	var fileexp=record.get('fileexp');
      var filesize=record.get('filesize');
      var fileexpmode=record.get('fileexpmode');
      var chatsoft=record.get('chatsoft');
      var chatsoftmode=record.get('chatsoftmode');
    

     	form.findField('chatsoftcombo').setValue(chatsoft);
     	form.findField('filecombo').setValue(fileexp);
     	form.findField('accountcombo').setValue(chataccount);
	    form.findField('filelimit').setValue(filesize);
     	form.findField('mode2').setValue({mode2:arr});
     	form.findField('mode').setValue({mode:fileexpmode});
     	form.findField('mode_account').setValue({mode_account:chatsoftmode});
     	form.findField('mode_chatsoft').setValue({mode_chatsoft:chatsoftmode});
   
   //  	form.findField('mode2').setValue({mode2:arr});
    }    
    else if(procytype=='cloud'){
    	if(contrcode&(0X00000001)){
     		arr.push('1');
     	}
      if(contrcode&(0X00000002)){
     		arr.push('2');
     	}    
     	form.findField('mode2').setValue({mode2:arr});
     	var cloudsoft=record.get('cloudsoft');
    
     	var cloudsoftmode=record.get('cloudsoftmode');

     	var fileexp=record.get('fileexp');
      var filesize=record.get('filesize');
      var fileexpmode=record.get('fileexpmode');
    

     	form.findField('cloudcombo').setValue(cloudsoft);
     	form.findField('filecombo').setValue(fileexp);
	    form.findField('filelimit').setValue(filesize);
     	form.findField('mode2').setValue({mode2:arr});
     	form.findField('mode').setValue({mode:fileexpmode});
     	form.findField('mode_cloudsoft').setValue({mode_cloudsoft:cloudsoftmode});

    }
     else if(procytype=='upan'){
    	if(contrcode&(0X00000001)){
     		arr.push('1');
     	}
      if(contrcode&(0X00000002)){
     		arr.push('2');
     	}    
     	form.findField('mode2').setValue({mode2:arr});
     	
     	var usbid=record.get('utype');
     	var umode=record.get('umode');
    

     	var fileexp=record.get('fileexp');
      var filesize=record.get('filesize');
      var fileexpmode=record.get('fileexpmode');
    

     	form.findField('usbid').setValue(usbid);
     	form.findField('filecombo').setValue(fileexp);
	    form.findField('filelimit').setValue(filesize);
     	form.findField('mode2').setValue({mode2:arr});
     	form.findField('mode').setValue({mode:fileexpmode});
     	form.findField('mode_usb').setValue({mode_usb:umode});

    }
     else if(procytype=='share'){
    	if(contrcode&(0X00000001)){
     		arr.push('1');
     	}
      if(contrcode&(0X00000002)){
     		arr.push('2');
     	}    
     	form.findField('mode2').setValue({mode2:arr});
     	
     	var fileout=record.get('fileout');
     	var fileread=record.get('fileread');
  

     	var fileexp=record.get('fileexp');
      var filesize=record.get('filesize');
      var fileexpmode=record.get('fileexpmode');
    

     	form.findField('filecombo').setValue(fileexp);
	    form.findField('filelimit').setValue(filesize);
     	form.findField('mode2').setValue({mode2:arr});
    
     	form.findField('mode').setValue({mode:fileexpmode});
    	if(fileout=='1'){
     	  form.findField('snap1').setValue('1');
      }
      else{
      	form.findField('snap1').setValue('0');
      }
      if(fileread=='1'){
    	   form.findField('snap2').setValue('1');
      }
      else{
      	form.findField('snap2').setValue('0');
      }

    }
     else if(procytype=='https'){
    	if(contrcode&(0X00000001)){
     		arr.push('1');
     	}
      if(contrcode&(0X00000002)){
     		arr.push('2');
     	}    
     	form.findField('mode2').setValue({mode2:arr});
    
     	form.findField('mode2').setValue({mode2:arr});
     	var hostmail=record.get('hostmail');
     	var httpname=record.get('httpname');
     	var fileexp=record.get('fileexp');
      var filesize=record.get('filesize');
      var fileexpmode=record.get('fileexpmode');
      var hostmailmode=record.get('hostmailmode');
      var httpmode=record.get('httpmode');
  
     	form.findField('httpscombo').setValue(httpname);
     	form.findField('filecombo').setValue(fileexp);
     	form.findField('serveripcombo').setValue(hostmail);
	    form.findField('filelimit').setValue(filesize);
     	form.findField('mode2').setValue({mode2:arr});
     	form.findField('mode').setValue({mode:fileexpmode});
     	form.findField('mode_serverip').setValue({mode_serverip:hostmailmode});
     	form.findField('mode_https').setValue({mode_https:httpmode});
   
   //  	form.findField('mode2').setValue({mode2:arr});
    }
    else if(procytype=='other'){
    	if(contrcode&(0X00000001)){
     		arr.push('1');
     	}
      if(contrcode&(0X00000002)){
     		arr.push('2');
     	}    
     	form.findField('mode2').setValue({mode2:arr});
     	
     	var fileout=record.get('utype');
     	var fileread=record.get('umode');
  

     	var fileexp=record.get('fileexp');
      var filesize=record.get('filesize');
      var fileexpmode=record.get('fileexpmode');
    

     	form.findField('filecombo').setValue(fileexp);
	    form.findField('filelimit').setValue(filesize);
     	form.findField('mode2').setValue({mode2:arr});
    
     	form.findField('mode').setValue({mode:fileexpmode});
     	form.findField('mode_othersoft').setValue({mode_othersoft:fileread});
     	
    	form.findField('othersoftcombo').setValue(fileout);

    }
    else if(procytype=='freeblock'){
   

      var freeapp=record.get('freeapp');
      var freeserver=record.get('freeserver');
      var blockapp=record.get('blockapp');
     	var blockserver=record.get('blockserver');
      form.findField('freecombo').setValue(freeapp);      
  
      form.findField('blockaddresscombo').setValue(blockserver);      
     	form.findField('freeaddresscombo').setValue(freeserver);

     	form.findField('blockcombo').setValue(blockapp);
     	
     	
    }
    
  },
  /***********************************
  *         ����������
  ***********************************/ 
  //���Ӳ���
  insertProcy: function(thisCmp, e, eOpts){
  
	 	if(thisCmp.up('form').getForm().isValid()){
	 		
	   	Ext.widget('groupcomtree').showAt(300,100);
	   }
  },
  //�������
  saveGroupCom: function(thisBtn){
    var selection = thisBtn.up('window').down('treepanel').getSelectionModel().getLastSelected();
    var controlcode=0;
    if(selection){
    	
    	var form = Ext.ComponentQuery.query('fileouttabs')[0].getActiveTab();
   
    	var record = form.getForm().getValues(); 
    	var hostmail = "";
    	var hostmailmode = 0;
    	var httpname = "";
    	var httpmode = 0;
    	var mailsend = "";
    	var mailRev = "";
    	var fileexp = "";
    	var fileexpmode = 0;
    	var filesize = 0;
    	var controlcode = 0;
    	var fileexp='';
    	var fileexpmode=0;
    	var filesize='';
    	var smtpsoft='';
    	var smtpsoftmode='';
    	var mailsendmode='';
    	var mailRevmode='';
    	var ftpname='';
    	var ftpmode='';
    	var ftpsoft='';
    	var ftpsoftmode='';
    	var account='';
    	var accountmode='';
    	var chatsoft='';
    	var chatsoftmode='';
    	var cloudsoft='';
    	var cloudsoftmode='';
    	var gridStore = Ext.ComponentQuery.query('fileoutlist')[0].getStore();
    	 switch(record.type){
        case 'advance':
    	      Ext.Array.each(form.down('checkboxgroup[name=forbidden]').getValue().forbidden,function(rec){
            controlcode += parseInt(rec); 
          });
          
          break;
        case 'http':
          hostmail = form.down('smtpadd').down('combo').getRawValue();       
          hostmailmode += form.down('smtpadd').down('radiogroup[name=mode_smtp]').getValue().mode_smtp;            
          httpname = form.down('httpsoft').down('combo').getRawValue();        
    	    httpmode += form.down('httpsoft').down('radiogroup[name=mode_soft]').getValue().mode_soft;    	   
    	    fileexp=form.down('file').down('combo').getRawValue();    	  
    	    fileexpmode += form.down('file').down('radiogroup[name=mode]').getValue().mode;        
          break;
         case 'smtp':
          hostmail = form.down('smtpadd').down('combo').getRawValue();
          hostmailmode += form.down('smtpadd').down('radiogroup[name=mode_smtp]').getValue().mode_smtp;          
          mailsend = form.down('sender').down('combo').getRawValue();
          mailRev = form.down('receiver').down('combo').getRawValue();      
          mailsendmode += form.down('sender').down('radiogroup[name=mode_sender]').getValue().mode_sender;
          mailRevmode += form.down('receiver').down('radiogroup[name=mode_receiver]').getValue().mode_receiver;
          smtpsoft = form.down('smtpsoft').down('combo').getRawValue();
    	    smtpsoftmode += form.down('smtpsoft').down('radiogroup[name=mode_smtpsoft]').getValue().mode_smtpsoft;
      
          break;
        case 'ftp':
        
          ftpname = form.down('ftpserver').down('combo').getRawValue();
          ftpmode += form.down('ftpserver').down('radiogroup[name=mode_ftp]').getValue().mode_ftp;         
          ftpsoft = form.down('ftpsoft').down('combo').getRawValue();
    	    ftpsoftmode += form.down('ftpsoft').down('radiogroup[name=mode_ftpsoft]').getValue().mode_ftpsoft;        
          break;
          
        case 'chat':
          account = form.down('account').down('combo').getRawValue();         
          accountmode += form.down('account').down('radiogroup[name=mode_account]').getValue().mode_account;                
          chatsoft = form.down('chatsoft').down('combo').getRawValue();                
    	    chatsoftmode += form.down('chatsoft').down('radiogroup[name=mode_chatsoft]').getValue().mode_chatsoft;    	   
          break;
        case 'cloud':
          chatsoft = form.down('cloudsoft').down('combo').getRawValue();
    	    chatsoftmode += form.down('cloudsoft').down('radiogroup[name=mode_cloudsoft]').getValue().mode_cloudsoft;

          break;
        case 'https':
          hostmail = form.down('serverip').down('combo').getRawValue();          
          hostmailmode += form.down('serverip').down('radiogroup[name=mode_serverip]').getValue().mode_serverip;
          httpname = form.down('httpssoft').down('combo').getRawValue();
    	    httpmode += form.down('httpssoft').down('radiogroup[name=mode_https]').getValue().mode_https;
   
          break;
        case 'upan':
          hostmail = form.down('combo[name=usbid]').getValue();       
          hostmailmode += form.down('radiogroup[name=mode_usb]').getValue().mode_usb;
        
          break;
        case 'share':
          if(form.down('checkboxgroup[name=snap]').getValue().snap1==1){
            hostmailmode = 1;
          }
          if(form.down('checkboxgroup[name=snap]').getValue().snap2==2){
            httpmode = 1;
          }
          break;
          case 'other':  
          hostmail = form.down('othersoft').down('combo').getRawValue();                  
          hostmailmode += form.down('radiogroup[name=mode_othersoft]').getValue().mode_othersoft;       
          break;
     
          
                   
        }
      
       if(form.down('file')){
        fileexp = form.down('file').down('combo').getRawValue();
        fileexpmode = form.down('file').down('radiogroup[name=mode]').getValue().mode;
        filesize += form.down('file').down('textfield[name=filelimit]').getValue();
 //       alert(filesize);
        Ext.Array.each(form.down('file').down('checkboxgroup[name=mode2]').getValue().mode2,function(rec){
          controlcode += parseInt(rec);
        });
      }      
        
   	gridStore.load({params:{
  	  deal: 'insert',
  	  filetype: record.type,
  	  hostmail: hostmail,
  	  hostmailmode: hostmailmode,
  	  httpname: httpname,
  	  httpmode: httpmode,
  	  mailsend: mailsend,
  	  mailRev: mailRev,
  	  mailRevmode:mailRevmode,
  	  mailsendmode:mailsendmode,
  	  smtpsoft:smtpsoft,
  	  smtpsoftmode:smtpsoftmode,
  	  fileexp: fileexp,
  	  fileexpmode: fileexpmode,
  	  filesize: filesize,
  	  controlcode: controlcode,
      groupid: selection.isLeaf()?"":selection.get('id'),
      compid: selection.isLeaf()?selection.get('id'):"",
  	  ftpname:ftpname,
  	  ftpmode:ftpmode,
  	  ftpsoft:ftpsoft,
  	  ftpsoftmode:ftpsoftmode,
  	  chataccount:account,
  	  chataccountmode:accountmode,
  	  chatsoft:chatsoft,
  	  chatsoftmode:chatsoftmode

    }});
  
      thisBtn.up('window').close();
    }
  },
  //�޸Ĳ���
  changeProcy: function(thisCmp, e, eOpts){ 
    if(parent.compid!='' || parent.groupid!=''){
    	var form = thisCmp.up('form');
    	var record = form.getForm().getValues(); 
    	var gridStore = Ext.ComponentQuery.query('fileoutlist')[0].getStore();    	
    	var hostmail = "";
    	var hostmailmode = 0;
    	var httpname = "";
    	var httpmode = 0;
    	var mailsend = "";
    	var mailRev = "";
    	var fileexp = "";
    	var fileexpmode = 0;
    	var filesize = 0;
    	var controlcode = 0;
    	var fileexp='';
    	var fileexpmode=0;
    	var filesize='';
    	var smtpsoft='';
    	var smtpsoftmode='';
    	var mailsendmode='';
    	var mailRevmode='';
    	var ftpname='';
    	var ftpmode='';
    	var ftpsoft='';
    	var ftpsoftmode='';
    	var account='';
    	var accountmode='';
    	var chatsoft='';
    	var chatsoftmode='';
    	var cloudsoft='';
    	var cloudsoftmode='';
  
      switch(record.type){
        case 'advance':
 //         controlcode
          Ext.Array.each(form.down('checkboxgroup[name=forbidden]').getValue().forbidden,function(rec){
            controlcode += parseInt(rec);
      //      alert(controlcode);
          });
          break;
        case 'http':      
          hostmail = form.down('smtpadd').down('combo').getRawValue();       
          hostmailmode += form.down('smtpadd').down('radiogroup[name=mode_smtp]').getValue().mode_smtp;            
          httpname = form.down('httpsoft').down('combo').getRawValue();        
    	    httpmode += form.down('httpsoft').down('radiogroup[name=mode_soft]').getValue().mode_soft;    	   
    	    fileexp=form.down('file').down('combo').getRawValue();    	  
    	    fileexpmode += form.down('file').down('radiogroup[name=mode]').getValue().mode;   	    	   
          break;
        case 'smtp':
          hostmail = form.down('smtpadd').down('combo').getRawValue();
          hostmailmode += form.down('smtpadd').down('radiogroup[name=mode_smtp]').getValue().mode_smtp;          
          mailsend = form.down('sender').down('combo').getRawValue();
          mailRev = form.down('receiver').down('combo').getRawValue();      
          mailsendmode += form.down('sender').down('radiogroup[name=mode_sender]').getValue().mode_sender;
          mailRevmode += form.down('receiver').down('radiogroup[name=mode_receiver]').getValue().mode_receiver;
          smtpsoft = form.down('smtpsoft').down('combo').getRawValue();
    	    smtpsoftmode += form.down('smtpsoft').down('radiogroup[name=mode_smtpsoft]').getValue().mode_smtpsoft;
      
          break;
        case 'ftp':
        
          ftpname = form.down('ftpserver').down('combo').getRawValue();
          ftpmode += form.down('ftpserver').down('radiogroup[name=mode_ftp]').getValue().mode_ftp;         
          ftpsoft = form.down('ftpsoft').down('combo').getRawValue();
    	    ftpsoftmode += form.down('ftpsoft').down('radiogroup[name=mode_ftpsoft]').getValue().mode_ftpsoft;        
          break;
          
        case 'chat':
          account = form.down('account').down('combo').getRawValue();         
          accountmode += form.down('account').down('radiogroup[name=mode_account]').getValue().mode_account;                
          chatsoft = form.down('chatsoft').down('combo').getRawValue();                
    	    chatsoftmode += form.down('chatsoft').down('radiogroup[name=mode_chatsoft]').getValue().mode_chatsoft;    	   
          break;
        case 'cloud':
          chatsoft = form.down('cloudsoft').down('combo').getRawValue();
    	    chatsoftmode += form.down('cloudsoft').down('radiogroup[name=mode_cloudsoft]').getValue().mode_cloudsoft;

          break;
        case 'https':
          hostmail = form.down('serverip').down('combo').getRawValue();          
          hostmailmode += form.down('serverip').down('radiogroup[name=mode_serverip]').getValue().mode_serverip;
          httpname = form.down('httpssoft').down('combo').getRawValue();
    	    httpmode += form.down('httpssoft').down('radiogroup[name=mode_https]').getValue().mode_https;
   
          break;
        case 'upan':
          hostmail = form.down('combo[name=usbid]').getValue();       
          hostmailmode += form.down('radiogroup[name=mode_usb]').getValue().mode_usb;
        
          break;
        case 'share':
          if(form.down('checkboxgroup[name=snap]').getValue().snap1==1){
            hostmailmode = 1;
          }
          if(form.down('checkboxgroup[name=snap]').getValue().snap2==2){
            httpmode = 1;
          }
          break;
          case 'other':  
          hostmail = form.down('othersoft').down('combo').getRawValue();                  
          hostmailmode += form.down('radiogroup[name=mode_othersoft]').getValue().mode_othersoft;       
          break;
          case 'freeblock':  
         
          hostmail = form.down('freesoftadd').down('combo').getRawValue();  
          smtpsoft=form.down('freeaddressadd').down('combo').getRawValue(); 
          mailsend=form.down('blocksoftadd').down('combo').getRawValue();
          mailRev=form.down('blockaddressadd').down('combo').getRawValue();   
         
          break;
      }
      if(form.down('file')){
        fileexp = form.down('file').down('combo').getRawValue();
        fileexpmode = form.down('file').down('radiogroup[name=mode]').getValue().mode;
        filesize += form.down('file').down('textfield[name=filelimit]').getValue();
 //       alert(filesize);
        Ext.Array.each(form.down('file').down('checkboxgroup[name=mode2]').getValue().mode2,function(rec){
          controlcode += parseInt(rec);
        });
      }
//      alert(controlcode);
  	if(controlcode==''&&record.type=='advance'){
  		alert('��ѡ����Ʋ���');
  		return;
  	}

   	gridStore.load({params:{
  	  deal: 'insert',
  	  filetype: record.type,
  	  hostmail: hostmail,
  	  hostmailmode: hostmailmode,
  	  httpname: httpname,
  	  httpmode: httpmode,
  	  mailsend: mailsend,
  	  mailRev: mailRev,
  	  mailRevmode:mailRevmode,
  	  mailsendmode:mailsendmode,
  	  smtpsoft:smtpsoft,
  	  smtpsoftmode:smtpsoftmode,
  	  fileexp: fileexp,
  	  fileexpmode: fileexpmode,
  	  filesize: filesize,
  	  controlcode: controlcode,
  	  compid: parent.compid,
  	  groupid: parent.groupid,
  	  ftpname:ftpname,
  	  ftpmode:ftpmode,
  	  ftpsoft:ftpsoft,
  	  ftpsoftmode:ftpsoftmode,
  	  chataccount:account,
  	  chataccountmode:accountmode,
  	  chatsoft:chatsoft,
  	  chatsoftmode:chatsoftmode

    }});
   
  }else{
    alert("��ѡ��Ҫ�·��Ķ���");
  }
 	

  },
  //ɾ������
  removeProcy: function(thisBtn){
 // 	var gridStore = Ext.ComponentQuery.query('fileoutlist')[0].getStore();
//gridStore.load({params:{
  	
 // 	filetype: 'advance'}});
  
    var grid = Ext.ComponentQuery.query('fileoutlist')[0];
    var selection = grid.getSelectionModel().getLastSelected();
    
    var procytype = "";
     var formValues = Ext.ComponentQuery.query('fileouttabs')[0].getActiveTab().getForm().getValues();
     procytype=formValues.type;  
 //   procytype = Fileout.controller.Fileout.switchType(selection.get('type'));
    
    if(selection){
      Ext.MessageBox.confirm('ȷ��', 'ȷ��ɾ���ü�¼��', function(btn){
				if(btn=='yes'){
				  var store = grid.getStore();
				  var compid = parent.compid;
				  var groupid = parent.groupid;
				  if(compid){
            groupid = "";
          }
          
					store.load({params:{
            deal: "delete",
            recno: selection.get('recno'),
            filetype:procytype,
            groupid: groupid,
            compid:  compid,
            seachflag: seachFlag
          }});
				}
			});
    }else{
      alert("��ѡ��Ҫɾ���Ĳ���!");
    }
  },
  //ɾ��ȫ����ʾ����
  removeAllProcy: function(){
    Ext.MessageBox.confirm('ȷ��', 'ȷ��ɾ����Щ��¼��', function(btn){                                                                                                                                                                                                                                                            
			if(btn=='yes'){
			  var store = Ext.ComponentQuery.query('fileoutlist')[0].getStore();
			  
			  var delprocy = "";
			  var compid = parent.compid;
			  var groupid = parent.groupid;
			  if(compid){
          groupid = "";
        }
			  var procytype = "";
        var formValues = Ext.ComponentQuery.query('fileouttabs')[0].getActiveTab().getForm().getValues();
        procytype=formValues.type;
			  
			  var i=0;
			  var ids='';
			  store.each(function(rec){
			   if(i==0){
			   	ids=rec.get('recno');
			  }
			  else{
			  	ids=ids+','+rec.get('recno');
			  }
		    i++;
	//		    delprocy += "("+LanProcy.controller.Procy.switchType(rec.get('procytype'))+","+rec.get('recno')+")";
			  });
			  
					store.load({params:{
            deal: "delall",
            recids: ids,
            filetype:procytype,
            groupid: groupid,
            compid:  compid,
            seachflag: seachFlag
          }});
			}
		});
  },
    /**************************************
  * ���Ա�ǩ�л� 
  ***************************************/  
  changeTab: function( tabPanel, newCard, oldCard, eOpts ){
  
  //   var formValues = newCard.getForm().getValues();
   //  alert(formValues.type);
      Fileout.controller.Fileout.loadFileoutListStore(parent.compid,parent.groupid);
  
  },
  /**************************************
  * ȫ�ֺ��� 
  ***************************************/
  inheritableStatics:{
    loadFileoutListStore:function(compid,groupid){
      var store = Ext.ComponentQuery.query('fileoutlist')[0].getStore();
     var formValues = Ext.ComponentQuery.query('fileouttabs')[0].getActiveTab().getForm().getValues();
     var ftype=formValues.type;    
      if(compid){
        groupid = "";
      }
      
      store.load({params:{
        groupid: groupid,
        compid:  compid,
        filetype:ftype
      }});
    },
    switchType: function(typeName){
      var procytype;
      
      switch(typeName){
        case '�ļ�����':
          procytype = 'file';
          break;
        case '��������':
          procytype = 'outlink';
          break;
        case 'Ӧ�ò���':
          procytype = 'app';
          break;
        case '�������':
          procytype = 'device';
          break;
        case '��ַ����':
          procytype = 'url';
          break;
        case '�������':
          procytype = 'net';
          break;
        case '��������':
          procytype = 'log';
          break;
      }
      
      return procytype;
    },
    setTitle: function(title){
      
      var grid = Ext.ComponentQuery.query('fileoutlist')[0];
      var oldtitle = grid.title;
      var str = "";
      var index = oldtitle.indexOf('(');
      if(index>0){
        str = oldtitle.substring(0,index);
      }else{
        str = oldtitle;
      }
      
      grid.setTitle(str+"("+title+")");
    }
  }
  
});