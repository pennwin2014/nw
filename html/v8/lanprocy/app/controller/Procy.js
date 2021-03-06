Ext.define('LanProcy.controller.Procy', {
  extend: 'Ext.app.Controller',
  
  stores: ['List','TimeCombo','SoftCombo','UsbCombo','SenseCombo','SenseGrid'],
	models: ['List','Combo','Params'],
  
  views: [
  	'common.TimeCombo','common.SoftCombo','common.UsbCombo','common.GroupComTree','common.TypeFieldset','common.SenseProcCombo','common.AddWin',
  	'list.List','tab.Main',
  	'tab.File','tab.Outlink','tab.App','tab.Device','tab.Url','tab.Net','tab.Log'
  ],

  init: function() {
  	
		this.control({
		  //策略列表
		  'procylist':{
		    select: this.showTab,
		    render: this.showRender
		  },
		  
		  //删除策略
		  'procylist button[action=remove]':{
				click: this.removeProcy
			},
			
			//删除全部显示策略
		  'procylist button[action=removeall]':{
				click: this.removeAllProcy
			},
		  
		  //策略标签
		  'procytabs':{
		    tabchange: this.changeTab
		  },
		  
		  //首选第一项
		  'procylist > store':{
		    load: this.selectFirst
		  },
		  
		  //部门电脑
		  'groupcomtree button[action=save]':{
		    click: this.saveGroupCom 
		  },
		  
			//文件控制
			'procyfile button[action=change]':{
				click: this.changeProcy
			},
			'procyfile button[action=insert]':{
				click: this.insertProcy
			},
			
			//外联控制
			'procyoutlink button[action=change]':{
				click: this.changeProcy
			},
			'procyoutlink button[action=insert]':{
				click: this.insertProcy
			},
			
			//网址控制
			'procyurl button[action=change]':{
				click: this.changeProcy
			},
			'procyurl button[action=insert]':{
				click: this.insertProcy
			},
			
			//网络控制
			'procynet textfield[name="text"]':{
				render: this.showTips
			},
			'procynet button[action=change]':{
				click: this.changeProcy
			},
			'procynet button[action=insert]':{
				click: this.insertProcy
			},
			
			//应用程序控制
			'procyapp button[action=change]':{
				click: this.changeProcy
			},
			'procyapp button[action=insert]':{
				click: this.insertProcy
			},
			
			//外设控制
			'procydevice button[action=change]':{
				click: this.changeProcy
			},
			'procydevice button[action=insert]':{
				click: this.insertProcy
			},
			
			//日志传送控制
			'procylog button[action=change]':{
				click: this.changeProcy
			},
			'procylog button[action=insert]':{
				click: this.insertProcy
			},
			
			//敏感进程屏幕
			'procylog typefieldset button[action=edit]':{
			  click: this.addProcWin
			},
			'addwin[id="addProc"] button[action=add]':{
			  click: this.insertProc
			},
			'procylog typefieldset button[action=add]':{
			  click: this.insertProcNameWin
			},
			'addwin[id="addProcName"] button[action=add]':{
			  click: this.insertProcName
			},
			'#senseProcCombo':{
			  change: this.searchProcValue
			},
			'procylog typefieldset button[action=mod]':{
			  click: this.modProcNameWin
			},
			'addwin[id="modProcName"] button[action=add]':{
			  click: this.modProcName
			},
			'procylog typefieldset button[action=del]':{
			  click: this.delProcName
			},
			'procylog typefieldset button[action=delall]':{
			  click: this.delallProcName
			}
			
		});
  },
  
  showRender: function(){
    //alert("here");
    //alert(parent.compid);
    LanProcy.controller.Procy.loadProcyListStore(parent.compid,parent.groupid);
    LanProcy.controller.Procy.setTitle(parent.dxtitle);
  },
  /**************************************
  * 策略入库操作
  ***************************************/ 
  //增加策略
  insertProcy: function(thisCmp, e, eOpts){
  	if(thisCmp.up('form').getForm().isValid()){
    	Ext.widget('groupcomtree').showAt(300,100);
    }
  },
  //保存策略
  saveGroupCom: function(thisBtn){
    var selection = thisBtn.up('window').down('treepanel').getSelectionModel().getLastSelected();
    
    if(selection){
      var store = Ext.ComponentQuery.query('procylist')[0].getStore();
      var formValues = Ext.ComponentQuery.query('procytabs')[0].getActiveTab().getForm().getValues();
      
      var controlmark = 0,controlmark2 = 0;
      if(formValues.controloffline){
        controlmark += 128;
      }
      
      Ext.Array.each(formValues.controlmark, function(item){
        controlmark += Number(item);
      });
      Ext.Array.each(formValues.controlmark2, function(item){
        controlmark2 += Number(item);
      });
      
      if(formValues.procytype1=='log'){
        var keyname=Ext.getCmp("senseProcCombo").getValue();
        var controlmark3 = Ext.getCmp("senseText2").getValue();
        //alert(controlmark3);
        store.load({params:{
          deal:"insert",
          procytype:formValues.procytype1,
          timeid: formValues.timeid,
          text: formValues.text,
          controlmark: controlmark,
          controlmark1: keyname,
          controlmark2: controlmark3,
          type: formValues.type,
          groupid: selection.isLeaf()?"":selection.get('id'),
          compid: selection.isLeaf()?selection.get('id'):"",
          seachflag: seachFlag
        }});
      }else{
        store.load({params:{
          deal:"insert",
          procytype:formValues.procytype1,
          timeid: formValues.timeid,
          softid: formValues.softid,
          usbid: formValues.usbid,
          text: formValues.text,
          controlmark: controlmark,
          controlmark1: formValues.controlmark1,
          controlmark2: controlmark2,
          snap: formValues.snap,
          type: formValues.type,
          groupid: selection.isLeaf()?"":selection.get('id'),
          compid: selection.isLeaf()?selection.get('id'):"",
          seachflag: seachFlag
        }});
      }
      thisBtn.up('window').close();
    }
  },
  //修改策略
  changeProcy: function(thisCmp, e, eOpts){
  	var form = thisCmp.up('form').getForm();
  	var record = form.getValues();
  	if(form.isValid()){
    	var grid = Ext.ComponentQuery.query('procylist')[0];
      var store = grid.getStore();
      
      var controlmark = 0,controlmark2 = 0;
      if(record.controloffline){
        controlmark += 128;
      }
      
      Ext.Array.each(record.controlmark, function(item){
        controlmark += Number(item);
      });
      Ext.Array.each(record.controlmark2, function(item){
        controlmark2 += Number(item);
      });
      if(parent.groupid == "" && parent.compid == ""){
        alert("未选择任何部门或计算机");
        return;
      }
      
     // alert(record.procytype1);
      if(record.procytype1=='log'){
        var keyname=Ext.getCmp("senseProcCombo").getValue();
        var controlmark3 = Ext.getCmp("senseText2").getValue();
        //alert(keyname);
        store.load({params:{
          deal:"insert",
          procytype:record.procytype1,
          timeid: record.timeid,
          text: record.text,
          controlmark: controlmark,
          controlmark1: keyname,
          controlmark2: controlmark3,
          type: record.type,
          groupid: parent.compid?"":parent.groupid,
          compid: parent.compid,
          seachflag: seachFlag
        }});
      }else{
        store.load({params:{
          deal:"insert",
          procytype:record.procytype1,
          timeid: record.timeid,
          softid: record.softid,
          usbid: record.usbid,
          text: record.text,
          controlmark: controlmark,
          controlmark1: record.controlmark1,
          controlmark2: controlmark2,
          snap: record.snap,
          groupid: parent.compid?"":parent.groupid,
          compid: parent.compid,
          type:record.type,
          seachflag: seachFlag
        }});
      }
      alert("应用成功");
    }
  },
  //删除策略
  removeProcy: function(thisBtn){
    var grid = Ext.ComponentQuery.query('procylist')[0];
    var selection = grid.getSelectionModel().getLastSelected();
    
    var procytype = "";
    
    procytype = LanProcy.controller.Procy.switchType(selection.get('procytype'));
    
    if(selection){
      Ext.MessageBox.confirm('确认', '确定删除该记录？', function(btn){
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
            procytype:procytype,
            groupid: groupid,
            compid:  compid,
            seachflag: seachFlag
          }});
				}
			});
    }else{
      alert("请选择要删除的策略!");
    }
  },
  //删除全部显示策略
  removeAllProcy: function(){
    Ext.MessageBox.confirm('确认', '确定删除这些记录？', function(btn){                                                                                                                                                                                                                                                            
			if(btn=='yes'){
			  var store = Ext.ComponentQuery.query('procylist')[0].getStore();
			  
			  var delprocy = "";
			  var compid = parent.compid;
			  var groupid = parent.groupid;
			  if(compid){
          groupid = "";
        }
			  
			  store.each(function(rec){
			    var i = 0;
			    
			    delprocy += "("+LanProcy.controller.Procy.switchType(rec.get('procytype'))+","+rec.get('recno')+")";
			  });
			  
				store.load({params:{
          deal: "delall",
          delprocy: delprocy,
          groupid: groupid,
          compid:  compid,
          seachflag: seachFlag
        }});
			}
		});
  },
  
  /**************************************
  * 策略列表 
  ***************************************/
  //默认选中第一个
  selectFirst: function(thisStore,records,successful,operation,eOpts ){
    //alert("store here");
    var selection = Ext.ComponentQuery.query('procylist')[0].getSelectionModel();
    
    if(selection){
      selection.select(1, true, false);
    }
  },
  
  //显示右侧Tab
  showTab: function(thisRM,record,index,eOpts){
    //alert("show tables");
    var procyTabs = Ext.ComponentQuery.query('procytabs')[0];
    var tab = 0;
    
    //var oldActiveTab = 0;
   // oldActiveTab = procyTabs.getActiveTab();
    
    switch(record.get('procytype')){
      case '文件策略':
        tab = 0;  
        break;
      case '外联策略':
        tab = 1;
        break;
      case '应用策略':
        tab = 2;
        break;
      case '外设策略':
        tab = 3;
        break;
      case '网址策略':
        tab = 4;
        break;
      case '网络策略':
        tab = 5;
        break;
      case '其他策略':
        tab = 6;
        break;
    }
    
    if(record.get('procytype') == procyTabs.getActiveTab().title){
      statu = 0;
    }else{
      statu = 1;
    }
    
   
    //console.log("old"+statu);
    procyTabs.setActiveTab(tab);

    //console.log(record);
    
    var controlmark = record.get('controlmark');
    var controlmark2 = record.get('controlmark2');
    var arr = new Array();
    var arr2 = new Array();
    
    for(var i=1;controlmark!=0;){
      
      if(controlmark%2){
        arr.push(+i+'');
      }
      i=i*2;
      controlmark = Math.floor(controlmark/2)
    }
    
    var formpanel = procyTabs.getActiveTab();
    var form = formpanel.getForm();
    if(tab==6){
      form.findField('type').setValue({type:record.get('type')});
      form.findField('controlmark').setValue({controlmark:arr});
      if(record.get('controlmark')>=128){
        form.findField('controloffline').setValue(true);
      }else{
        form.findField('controloffline').setValue(false);
      }
      Ext.getCmp("senseProcCombo").select(controlmark2);
      Ext.getCmp("senseText2").setValue(record.get('controlmark1'));
      return ;
    }
    
    for(var i=1;controlmark2!=0;){
      
      if(controlmark2%2){
        arr2.push(+i+'');
      }
      i=i*2;
      controlmark2 = Math.floor(controlmark2/2)
    }
    
    //console.log(arr);
    form.loadRecord(record);

    try{
      form.findField('snap').setValue({snap:record.get('snap')});
      form.findField('type').setValue({type:record.get('type')});
      form.findField('controlmark').setValue({controlmark:arr});
    }catch(e){}
      
    try{
      form.findField('controlmark1').setValue({controlmark1:record.get('controlmark1')});
    }catch(e){}
    
    try{
      form.findField('controlmark2').setValue({controlmark2:arr2});
    }catch(e){}
      
    if(record.get('procytype')=='网络策略'){
      form.findField('controlmark').setValue({snap:record.get('controlmark')});
    }
    
    console.log(record.get('levelremain'));
    if(record.get('levelremain')<0){
      formpanel.down('button[action="change"]').setDisabled(true);
    }
  },
  /**************************************
  * 策略标签切换 
  ***************************************/  
  changeTab: function( tabPanel, newCard, oldCard, eOpts ){
    
    //console.log("new"+statu);
    if(statu==0){
      seachFlag = 2;
      var store = Ext.ComponentQuery.query('procylist')[0].getStore();
      var formValues = newCard.getForm().getValues();
      
      store.load({params:{
        procytype:formValues.procytype1,
        groupid: parent.groupid,
        compid: parent.compid
      }});
    }
    
    statu = 0;
  },
  
  /**************************************
  * 策略标签 
  ***************************************/  
  showTips: function( thisTV, eOpts ){
    thisTV.tip = Ext.create('Ext.tip.ToolTip', {
      target: thisTV.el,
      trackMouse: true,
      dismissDelay : 60000,
      html: '<p>例如：</p><p>&nbsp;&nbsp;&nbsp;单个端口：80,21</p><p>&nbsp;&nbsp;&nbsp;端口段：2000-3000</p><p>&nbsp;&nbsp;&nbsp;组合：80,2000-3000,3005</p>'
    });
  },
  
  /**************************************
  * 敏感进程屏幕 
  ***************************************/
  addProcWin:function(){
    Ext.create('LanProcy.view.common.AddWin',{
      id: 'addProc',
      field1: '进程类别'
    }).showAt(350,100);
  },
  insertProc:function(btn){
    var procname = btn.up('window').down('form').getForm().findField('field1').getValue();
    Ext.Ajax.request({
	    url: '/pronline/Msg',
	    params: {
	    	FunName: "lan_dealParams",
	    	deal:"insert",
	    	paramtype: 2,
	    	keyname:procname
	    },
	    success: function(response){
	    	var text = Ext.decode(response.responseText); 
	    	if(text.value=='failure'){
	    	 	alert("该名称已经存在！");
	    	}else{
	    		alert("添加成功！");
	    		Ext.getCmp("senseProcCombo").getStore().load();
	    		btn.up('window').close();
	    	}
	    }
		});
  },
  insertProcNameWin:function(btn){
    Ext.create('LanProcy.view.common.AddWin',{
      id: 'addProcName',
      title: "增加",
      field1: '进程名',
      field2: '窗口标题关键词'
    }).showAt(350,100);
  },
  insertProcName:function(btn){
    var form = btn.up('window').down('form').getForm();
    var keyname=Ext.getCmp("senseProcCombo").getRawValue();
    var grid=Ext.getCmp('senseGrid');
    //alert(form.findField('field1').getValue());
    //alert(form.findField('field2').getValue());
    Ext.Ajax.request({
	    url: '/pronline/Msg',
	    params: {
	    	FunName: "lan_dealParams",
	    	deal:"insert",
	    	paramtype: 2,
	    	keyname:keyname,
	    	keyvalue:form.findField('field1').getValue(),
	    	detail:form.findField('field2').getValue()
	    },
	    success: function(response){
	    	var text = Ext.decode(response.responseText); 
	    	if(text.value=='false'){
	    	 	alert("该名称已经存在！");
	    	}else{
	    		alert("添加成功！");
	    		btn.up('window').close();
	    		grid.getStore().load({params:{paramtype:2,keyname:keyname}});
	    	}
	    }
		});
  },
  modProcNameWin:function(btn){
    var grid=Ext.getCmp('senseGrid');
    var selection = grid.getSelectionModel().getLastSelected();
    if(selection){
      Ext.create('LanProcy.view.common.AddWin',{
        id: 'modProcName',
        title: "修改",
        value1: selection.get('recno'),
        field1: '进程名',
        field2: '窗口标题关键词'
      }).showAt(350,100);
      var form = Ext.getCmp('modProcName').down('form').getForm();
      form.findField('field1').setValue(selection.get('name'));
      form.findField('field2').setValue(selection.get('detail'));
    }else{
      alert("请选择要修改项!");
    }
  },
  modProcName:function(btn){
    var form = btn.up('window').down('form').getForm();
    var keyname=Ext.getCmp("senseProcCombo").getRawValue();
    var grid=Ext.getCmp('senseGrid');
    Ext.Ajax.request({
	    url: '/pronline/Msg',
	    params: {
	    	FunName: "lan_dealParams",
	    	deal:"update",
	    	paramtype: 2,
	    	recno:form.findField('value1').getValue(),
	    	keyname:keyname,
	    	keyvalue:form.findField('field1').getValue(),
	    	detail:form.findField('field2').getValue()
	    },
	    success: function(response){
	    	var text = Ext.decode(response.responseText); 
	    	if(text.value=='false'){
	    	 	alert("该名称已经存在！");
	    	}else{
	    		alert("修改成功！");
	    		grid.getStore().load({params:{paramtype:2,keyname:keyname}});
	    		btn.up('window').close();
	    	}
	    }
		});
  },
  searchProcValue:function(thisField,newValue,oldValue,eOpts){
    var keyname=Ext.getCmp("senseProcCombo").getRawValue();
    var grid=Ext.getCmp('senseGrid');
    grid.getStore().load({params:{paramtype:2,keyname:keyname}});
  },
  delProcName: function(btn){
    var grid=Ext.getCmp('senseGrid');
    var selection = grid.getSelectionModel().getLastSelected();
    var keyname=Ext.getCmp("senseProcCombo").getRawValue();
    if(selection){
      Ext.create('Ext.window.Window',{
        title: "删除",
        modal:true,
        width: 250,
        height: 100,
        //icon: Ext.Msg.QUESTION,
        html: '确认要删除选择项?',
        buttons:[{
          text: '确定',
          handler: function(btn){
            Ext.Ajax.request({
        	    url: '/pronline/Msg',
        	    params: {
        	    	FunName: "lan_dealParams",
        	    	deal:"delete",
        	    	recno: selection.get('recno')
        	    },
        	    success: function(response){
        	    	var text = Ext.decode(response.responseText); 
        	    	if(text.value=='false'){
        	    	 	alert("删除失败！");
        	    	}else{
        	    		alert("删除成功！");
        	    		grid.getStore().load({params:{paramtype:2,keyname:keyname}});
        	    		btn.up('window').close();
        	    	}
        	    	btn.up('window').close();
        	    }
        		});
        		
          }
        },{
          text:"取消",
          handler: function(btn){
            btn.up('window').close();
          }
        }]
      }).showAt(300,50);
    }else{
      alert("请选择要删除项!");
    }
    
  },
  delallProcName:function(btn){
    var grid=Ext.getCmp('senseGrid');
    var store=grid.getStore();
    var keyname=Ext.getCmp("senseProcCombo").getRawValue();
    var recno = "";
    var i=0;
    store.each(function(rcd){
      if(i>0){
        recno+=',';
      }
      recno+=rcd.get('recno');
      i++;
    });
    Ext.create('Ext.window.Window',{
      title: "删除",
      modal:true,
      width: 250,
      height: 100,
      //icon: Ext.Msg.QUESTION,
      html: '确认要删除选择项?',
      buttons:[{
        text: '确定',
        handler: function(btn){
          Ext.Ajax.request({
      	    url: '/pronline/Msg',
      	    params: {
      	    	FunName: "lan_dealParams",
      	    	deal:"delall",
      	    	recno: recno
      	    },
      	    success: function(response){
      	    	var text = Ext.decode(response.responseText); 
      	    	if(text.value=='false'){
      	    	 	alert("删除失败！");
      	    	}else{
      	    		alert("删除成功");
      	    		grid.getStore().load({params:{paramtype:2,keyname:keyname}});
      	    		btn.up('window').close();
      	    	}
      	    }
      		});
        }
      },{
        text:"取消",
        handler: function(btn){
          btn.up('window').close();
        }
      }]
    }).showAt(300,50);
  },
  
  /**************************************
  * 全局函数 
  ***************************************/
  inheritableStatics:{
    loadProcyListStore:function(compid,groupid){
      var store = Ext.ComponentQuery.query('procylist')[0].getStore();
      
      if(compid){
        groupid = "";
      }
      
      store.load({params:{
        groupid: groupid,
        compid:  compid
      }});
    },
    switchType: function(typeName){
      var procytype;
      
      switch(typeName){
        case '文件策略':
          procytype = 'file';
          break;
        case '外联策略':
          procytype = 'outlink';
          break;
        case '应用策略':
          procytype = 'app';
          break;
        case '外设策略':
          procytype = 'device';
          break;
        case '网址策略':
          procytype = 'url';
          break;
        case '网络策略':
          procytype = 'net';
          break;
        case '日志传送策略':
          procytype = 'log';
          break;
      }
      
      return procytype;
    },
    setTitle: function(title){
      
      var grid = Ext.ComponentQuery.query('procylist')[0];
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
