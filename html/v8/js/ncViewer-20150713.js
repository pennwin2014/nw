


Ext.Loader.setConfig({enabled: true,paths:{'ncViewer':'/v8/js'}});
Ext.define('ncViewer.App', {
    extend: 'Ext.container.Viewport',    
    uses:['ncViewer.ncGridUser','ncViewer.lanProcy','ncViewer.lanSsjk','ncViewer.lanRzck','ncViewer.lanComp','ncViewer.lanBase','ncViewer.lanProcyXf','ncViewer.lanSystem','ncViewer.lanSoftUpdate','ncViewer.lanDeptment','ncViewer.lanGroupTree','ncViewer.lanGroupCheckTree','ncViewer.lanSysSet','ncViewer.lanInstall','ncViewer.lanOutfile'],
    initComponent: function(){
        Ext.define('State', {
            extend: 'Ext.data.Model',
            fields: ['groupid', 'groupname']
        });       
        Ext.apply(this, {
            layout: {
                type: 'border',
                padding: 2
            },
            items: [this.createNcMenu(),this.createNcTop(),this.createNcCenter(),this.createNcStatus()]
        });
        this.callParent(arguments);
    },
  
    /**
    ����������
     */
    createNcMenu: function(){
        var right_fun_s=right_fun;

		//�����������ݲ˵�
		var menu_base = Ext.create('Ext.menu.Menu', {
				id: 'baseMenu',
				style: {
					overflow: 'visible'  
				},
				items: [
				  {
					  text: '�ͻ�����Ϣ',
					  itemid:'clinetinfo',
					  srcurl:'ncViewer.lanClient',  
					  handler:this.onItemClick
			   
				   },
				   {
						text: '���Ź���',
						itemid:'deptment',
						srcurl:'ncViewer.lanDeptment',
					   handler:this.onItemClick
				   },
				   {
						text: 'Ԥ�������',
						itemid:'baseinfo',
					   srcurl:'ncViewer.lanBase',
					   handler:this.onItemClick
				   }
				]
			});

		menu_base.items.each(function(item){
			if(right_fun_s.indexOf(item.text)== -1){
				item.destroy();
			}
		});
		

		var tb = Ext.create('Ext.toolbar.Toolbar',{id: "maintab"});
    	tb.add({
			itemid:'jcxxgl',
			hidden:false,
			text:'������Ϣ',
			scale:'large',
			iconCls: 'macs',
			menu:menu_base,
			iconAlign: 'top'
		}); 
		var isep=0;

		tb.items.each(function(item){
			if((right_fun_s.indexOf(item.text)!=-1)||(item.text=='ϵͳˢ��')||(item.text=='���µ�¼')||item.itemid=='logo'){
			
				item.hidden=false;
				isep=1;
			 
			}
			else{
				if(item.text){
					item.hidden=true;
					isep=0;
				
				}
				else{
					if(isep==0){
					 item.hidden=true;
					 isep=1;
					}
					else{
						item.hidden=false;
						isep=0;
					}
				}
			}			 
		});  
		
		
		
		this.toolPanel = Ext.create("Ext.panel.Panel", {
                layout: "accordion",  //����Ϊ�ַ��ٲ���
                layoutConfig: {
                    animate: true
                },
                width: 250,
                minWidth: 90,
                region: "west", //���÷�λ
                split: true,
				items:[
						{
							title: '������ѯ',
							height: 300,
							width: 250,
							layout: 'column',
							defaults:
								{                     
									layout: 'anchor', 
									defaults: {   anchor: '100%'  } 
								},
							 items:
							 [
								{  
									columnWidth: 10 / 10,        //�����еĿ��
									items:
									[
										{
											id : 'image_button6', 
											width:300,
											allowDepress:true,
											//enableToggle:true,
											xtype : 'button',
											text : '������ѯ1',
											itemid:'clinetinfo',
											srcurl:'ncViewer.lanClient',  
											listeners:{
												click:this.onItemClick,
											}
										},{
											id : 'image_button7', 
											width:300,
											allowDepress:true,
											//enableToggle:true,
											xtype : 'button',
											text : '������ѯ2',
											itemid:'clinetinfo',
											srcurl:'ncViewer.lanClient',  
											listeners:{
												click:this.onItemClick,
											}
										},{
											id : 'image_button8', 
											width:300,
											allowDepress:true,
											//enableToggle:true,
											xtype : 'button',
											text : '������ѯ3',
											itemid:'clinetinfo',
											srcurl:'ncViewer.lanClient',  
											listeners:{
												click:this.onItemClick,
											}
										}

									]
								}
							]
						},{
							title: '�������',
							height: 300,
							width: 250,
							layout: 'column',
							defaults:
								{                     
									layout: 'anchor', 
									defaults: {   anchor: '100%'  } 
								},
							 items:
							 [
								{  
									columnWidth: 10 / 10,        //�����еĿ��
									items:
									[
										{
											id : 'image_button3', 
											width:300,
											allowDepress:true,
											//enableToggle:true,
											xtype : 'button',
											text : '������ѯ',
											itemid:'clinetinfo',
											srcurl:'ncViewer.lanClient',  
											listeners:{
												click:this.onItemClick,
											}
										},
										{ 
											title: "������ѯ", 
											layout: "accordion", 
											iconCls: "danganchaxun", 
											itemid:'deptment',
											srcurl:'ncViewer.lanDeptment',
											listeners:{
												'collapse': function() {
													alert("������ѯ����"); 
												},
												'expand': this.onItemClick,
											},
											items:[
													{ 
														title: "�ն˵���", 					
														itemid:'deptment',
														collapsible:false, 
														hideCollapseTool:true,	
														srcurl:'ncViewer.lanDeptment',
														listeners:{
															'collapse': function() {
																alert("�����ն˵���"); 
															},
															'expand': this.onItemClick,
														}
													},{
														title: "��������",
														collapsible:false, 
														hideCollapseTool:true,	
														itemid:'clinetinfo',
														srcurl:'ncViewer.lanClient',  
														listeners:{
															'collapse': function() {
																alert("������������"); 
															},
															'expand': this.onItemClick,
														}
													},{ 
														title: "��Ա����", 
														collapsible:false, 
														hideCollapseTool:true,	
														itemid:'baseinfo',
														srcurl:'ncViewer.lanBase',
														listeners:{
															'collapse': function() {
																alert("������Ա����"); 
															},
															'expand': this.onItemClick,
														}
													}
												]
										},{ 
											title: "���ظ澯", 
											layout: "accordion", 
											iconCls: "bukonggaojing",
											itemid:'deptment',
											srcurl:'ncViewer.lanDeptment',
											listeners:{
												'collapse': function() {
													alert("���ظ澯����"); 
												},
												'expand': this.onItemClick,
											},
											items:[
													{ 
														title: "���ع���", 		
											collapsible:false, 
											hideCollapseTool:true,															
														itemid:'deptment',
														srcurl:'ncViewer.lanDeptment',
														listeners:{
															'collapse': function() {
																alert("�������ع���"); 
															},
															'expand': this.onItemClick,
														}
													},{
														title: "�Ӿ���Ա����",
														collapsible:false, 
														hideCollapseTool:true,	
														itemid:'clinetinfo',
														srcurl:'ncViewer.lanClient',  
														listeners:{
															'collapse': function() {
																alert("�����Ӿ���Ա����"); 
															},
															'expand': this.onItemClick,
														}
													},{ 
														title: "������Ϊ����", 
														collapsible:false, 
														hideCollapseTool:true,	
														itemid:'baseinfo',
														srcurl:'ncViewer.lanBase',
														listeners:{
															'collapse': function() {
																alert("����������Ϊ����"); 
															},
															'expand': this.onItemClick,
														}
													}
												]
										}
									]
								}
							]
						},{ 
							title: "��ҳ", 
							iconCls: "shouye", 
							height:100,
							itemid:'deptment',
							srcurl:'ncViewer.lanDeptment',
							listeners:{
								'collapse': function() {
									alert("��ҳ����"); 
								},
								'expand': this.onItemClick,
							},
							items:[
							    {
									id : 'image_button1', 
									width:300,
									allowDepress:true,
									//enableToggle:true,
									xtype : 'button',
									text : 'button',
									itemid:'clinetinfo',
									srcurl:'ncViewer.lanClient',  
									listeners:{
										click:this.onItemClick,
									}
								},{
									id : 'image_button2',
									width:300,
									allowDepress:true,
									//enableToggle:true,
									xtype : 'button',
									text : 'er',
									itemid:'baseinfo',
									srcurl:'ncViewer.lanBase',
									listeners:{
										click:this.onItemClick,
									}
								}
							]
							
						},{ 
							title: "����", 
							iconCls: "shouye", 
							itemid:'deptment',
							height:100,
							srcurl:'ncViewer.lanDeptment',
							listeners:{
								'collapse': function() {
									alert("��ҳ����"); 
								},
								'expand': this.onItemClick,
							},
							items:[
								{
									id : '333', 
									width:300,
									allowDepress:true,
									enableToggle:true,
									xtype : 'button',
									text : '333',
									itemid:'clinetinfo',
									srcurl:'ncViewer.lanClient',  
									listeners:{
										click:this.onItemClick,
									}
								},{
									id : '444', 
									width:300,
									allowDepress:true,
									enableToggle:true,
									xtype : 'button',
									text : '444',
									itemid:'baseinfo',
									srcurl:'ncViewer.lanBase',
									listeners:{
										click:this.onItemClick,
									}
								}
							]
							
						}
					]
            });
			
		return this.toolPanel;
    },
    onItemClose:function(){
    	self.location='/login2.htm';
    },
    onItemReflash:function(){
          var fp=Ext.create('Ext.FormPanel');
               fp.form.doAction('submit',{
               url:'/pronline/Msg',
                method:'POST',
               params:{FunName:'ncsSysReflesh_v4'},
                 
                success:function(form,action){ 
           					Ext.getCmp('winstatus').close();
                 },
             		 failure:function(form,action){        		 
                    Ext.Msg.alert('����',"ϵͳ��æ,�Ժ�����!");
                    Ext.getCmp('winstatus').close();
            		 }
                }
             )
					    var wint = Ext.create('Ext.Window', {
					 //       title: 'ϵͳˢ��',
					        id:'winstatus',
					        header:false,
					        width: 150,
					        x:600,
					        y:210,
					        height: 40,
					        closable: false,
					        modal: true,
					  //      closeAction: 'hide',
					        bodyPadding: 10,
					        bbar: Ext.create('Ext.ux.StatusBar', {
					            id: 'win-statusbar',
					            defaultText: 'ϵͳ����'
					        })
					    });					   
					    wint.show();
    },
    createNcTop:function(){       
		this.topPanel=Ext.create(Ext.panel.Panel, {
            id: 'left',
			height: 120,
			border: true,
            margins: '0 2 0 2',
            region: 'north',
			unstyled:true,
			bodyStyle:"background-color: white; background-image: url(/images/mac/back.png);background-repeat:no-repeat;background-position:left;",
			html:"",
			items:[]
        });
        return this.topPanel;
    },
    createNcCenter:function(){
		groupid='';
		compid='';
		dxtitle='';
    	this.centerPanel=Ext.createWidget('tabpanel', {
        resizeTabs: true,
        enableTabScroll: true,
			region:'center',
        	id:'layout_center',
        	margins: '2 2 1 0',
			defaults: {
				autoScroll: false,
				bodyPadding: 0
			},
			plugins: Ext.create('Ext.ux.TabCloseMenu', {
				extraItemsTail: [
					'-',
					{
						text: 'Closable',
						checked: true,
						hideOnClick: true,
						handler: function (item) {
							currentItem.tab.setClosable(item.checked);
						}
					},
					'-',
					{
						text: 'Enabled',
						checked: true,
						hideOnClick: true,
						handler: function(item) {
							currentItem.tab.setDisabled(!item.checked);
						}
					}
				],
				listeners: {
					aftermenu: function () {
						currentItem = null;
					},
					beforemenu: function (menu, item) {
						menu.child('[text="Closable"]').setChecked(item.closable);
						menu.child('[text="Enabled"]').setChecked(!item.tab.isDisabled());

						currentItem = item;
					}
				}
			}),
			listeners: {              
				tabchange:function(tp,p){
					var tabs_center=Ext.getCmp("layout_center");
					var active = tabs_center.getActiveTab();
					var active_id='lan_'+active.id;
					var tabs_if=Ext.getCmp(active_id).getActiveTab();
					if(tabs_if){
						if(tabs_if.id=='immon'){
							bStartcomp=0;
							bStartim=1;
						}else if(tabs_if.id=='sjcomputer'){
							bStartcomp=1;
							bStartim=0;
						}else if(tabs_if.id=='sspm'){
							bStartcomp=0;
							bStartim=0;
						}
						var vframe= active_id+"_"+tabs_if.getId()
						//alert(vframe);
						try{
						  eval(vframe).reflash();
						}catch(e){}
					}
				}
			},items:[
					{
						title: '�ڰ�����',
						id:'blacklist', 
						html: '<iframe id="lan_procy_blacklist" name="lan_procy_blacklist" src="/v8/lanprocy/lan_procy_blacklist.html" scrolling="no" width=100% height=650 FrameBorder=0></iframe>' 
					}
				]
                
		});
		var right_fun_s=right_fun;
		var item2 = Ext.getCmp("maintab").items.items;   
		for(var i=0;i<item2.length;i++){
			try{
				if((right_fun_s.indexOf(item2[i].text)!=-1)){
					if(item2[i].itemid=="jcxxgl"){
						continue;
					}
					this.onItemClick(item2[i]);
					break;
				}
			}catch(e){}  
		}
        return this.centerPanel;
    },
    createNcStatus:function(){   	
    	this.ncstatus=Ext.create('Ext.ux.StatusBar', {
			id: 'statusbar',
			region: 'south',
			margins: '1 2 0 2',
			defaultText: 'Default status text',
			text: 'Ready',
			iconCls: 'x-status-valid',
			items: []
		});
		return this.ncstatus;
    },
    onItemClick:function(item){
		//�ı��м����ͼ      
		var i=0;
		var m=0;
		var tab_title=item.text;
		var tab_src=item.srcurl;
		var tab_id=item.itemid;
		var tabs_center=Ext.getCmp("layout_center");
		var tab_len=tabs_center.items.length;
		tabs_center.items.each(function(item) { 
	     	if(item.id==tab_id)
	     	{ i++;}
			if(i==0){m++;}
		});   
		if(i>0){
			tabs_center.setActiveTab(m);
			return;
     	} 
		if(tab_len>5){
			tabs_center.remove(1);  
		}	  
		var grid=Ext.create(tab_src);
		//Ȩ�޴���
		var str = right_fun;
		grid.items.each(function(item){
			if(str.indexOf(item.getId())== -1){
				item.destroy();
			}
		});  
		
		tabs_center.add({
			closable: true,
			autoScroll:true,
			id:tab_id,
			//iconCls: 'add16',          
			title: tab_title,
			items:grid
			//items:[grid]
		}).show();
    },
    showTabs:function(parentTab,childTab){
    	
    },
    refreshComTree: function(){
		Ext.getCmp('treeBtn').fireEvent('click');
    }
});
