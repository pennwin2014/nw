/**
 ���岿����
 */
 
 Ext.define('ncViewer.lanGroupTree', {
    extend: 'Ext.tree.Panel',
    alias: 'widget.lanGroupTree',
   
    change: function(val) {
        if (val > 0) {
            return '<span style="color:green;">' + val + '</span>';
        } else if (val < 0) {
            return '<span style="color:red;">' + val + '</span>';
        }
        return val;
    },
    initComponent: function(){
        var cstate='';
    	  Ext.define('treedata',{
		        extend: 'Ext.data.Model',
		        fields: [
		            {name: 'text',    type: 'string'},
		            {name: 'id',      type: 'string'},
		            {name: 'compid',  type: 'string'},
		            {name: 'ctext',   type: 'string'}
		
		        ]
        });
		    store_comp = Ext.create('Ext.data.TreeStore', {
		   	    model:'treedata',
		        proxy: {
		            type: 'ajax',
		            url: '/pronline/Msg?FunName@lanTree_depComp'
		        },
		        root: {
		            text: 'ȫ��',
		            id: 0,
		            expanded: true
		        },
		        folderSort: true,
		        sorters: [{
		 //           property: 'text',
		//            direction: 'ASC'
		        }],
		        listeners:{
		          'load':function(thisTS,node,records,successful,eOpts){
		            //alert(records[0].get('text'));
		            if(records && records[0] && records[0].get('text')=='ȫ��'){
		              //alert("/0/"+records[0].get('id'));
  		            Ext.getCmp('groupTree').expandPath("/0/"+records[0].get('id'));
  		          }		            
		          }
		        }
		    });
		    store_comp.on('beforeload', function (store, options) {
		
		        var new_params = { state: Ext.getCmp('compstate').getValue() };
		        Ext.apply(store.proxy.extraParams, new_params);
		    });
    
    var compstate = [
        {"groupid":"","groupname":"״̬2"},
        {"groupid":"1","groupname":"����"},
        {"groupid":"2","groupname":"����"}
     
    ];
    
    var store_compstate = Ext.create('Ext.data.Store', {
        model: 'State',
        data: compstate
    });
    var compstateCombo = Ext.create('Ext.form.field.ComboBox', {
        fieldLabel: '',
        labelWidth: 50,
        width: 50,
        valueField:'groupid',
        displayField: 'groupname',
        name : 'compstate',      
        value:'',
        id:'compstate',
        allowBlank: true,
        forceSelection: false,
        store: store_compstate,
        queryMode: 'local',
        typeAhead: true,
        listeners: {
			          'change':function(){			     
			          		var vkeyword=Ext.getCmp('keyword').value;				                
                    if(vkeyword=='�������/IP/MAC'){
                    	vkeyword='';
                    }
		//	          	var vstate=Ext.getCmp('compstate').value;
			          	  store_comp.load({params: {keyword:vkeyword}});
			          }
			  }
    });
    	
   



    Ext.apply(this, {
 //       title: '���ŵ���',
    	  margins: '0 0 0 0',
    	  id:'groupTree',
        store: store_comp,
        autoScroll: true,
         border:false,
        forceFit: true,
        rootVisible: false,
   //     width: 110,
        useArrows: false,
        dockedItems: [{
            xtype: 'toolbar',
            items: [compstateCombo,
            {
               xtype:'textfield',
               id:'keyword',
               name:'keyword',
               style:'color:#7aa7d5',
               width:110,
               emptyText:'�������/IP/MAC',
               enableKeyEvents:true,
               listeners:{
                 'focus':function(){
                 if(this.value==''){                
                    this.setValue('');
                  }
                 },
         
                 'keydown' : function(i,e){
                var aae=e.getKey() ; 
                if(aae==13){
                    var aa=Ext.getCmp('keyword').value;
                    if(aa!=''){
                    	  var vkeyword=Ext.getCmp('keyword').value;			         
						          	store_comp.load({params: {keyword:vkeyword}});

		                   }
                }
               }
               }
             },
             
              {
                text:'',
                itemId: 'moveButton',
                iconCls:'accept',
 //               disabled: true,
 								id:"treeBtn",
               /* handler:function(){
                	  var aa=Ext.getCmp('keyword').value;
                    if(aa!=''){
                        var vkeyword=Ext.getCmp('keyword').value;			         
						          	store_comp.load({params: {keyword:vkeyword}});
						          }
   
                },*/
                listeners: {
                	'click': function(){
                		var aa=Ext.getCmp('keyword').value;
                    var vkeyword=Ext.getCmp('keyword').value;			         
				          	store_comp.load({params: {keyword:vkeyword}});
                	}
                }
 
                
            }
             
             
             
             ]}]
              ,        
        listeners:{
         	itemclick:function(view, rcd, item, idx, event, eOpts){
         	compidTmp = "";
         	compNameTmp = "";
         	
        	compid=rcd.get('compid');
         	if(compid==""){
        	  groupid=rcd.get('id');        
        	}else{
        	  groupid = "";
        	}
        	if(groupid!=''){
        	dxtitle='���ţ�'+rcd.get('text');
        	dxtitle_c='���ţ�'+rcd.get('ctext');
          }
          if(compid!=''){
          	dxtitle='�������'+rcd.get('text');
          	dxtitle_c='�������'+rcd.get('ctext');
          }
          if(groupid==''&&compid==''){
          	dxtitle='';
          	dxtitle_c='';
          }
          try{
	        	var tabs_center=Ext.getCmp("layout_center");
	        	var active = tabs_center.getActiveTab();
	        	
	        	var active_id='lan_'+active.id;
	        	//alert(active_id);
	        	var tabs_if=Ext.getCmp(active_id).getActiveTab();
	        	//alert(tabs_if);
	        	var tabframe=active_id+'_'+tabs_if.id;
	        	//alert(tabframe);
	        	
	          eval(tabframe).SetPage();
	        	eval(tabframe).reflash();
	          
	          
				}catch(e){
					
				}
         }, 
				'itemcontextmenu':function(thisItem,record,item,index,event,eOpts){
        		event.stopEvent();
        		var cmpStatus;
        		
        		if(record.isLeaf()){
        			if(record.get('text').indexOf('����')>0){
        				cmpStatus = true;
        			}else{
        				cmpStatus = false;
        			}
	        		Ext.create('Ext.menu.Menu', {
							floating:	true,
							items: [{
								xtype: 'label',
								text: '�鿴�������Ϣ',
								padding: '5 5 5 30',
								cls: "menuTitle"
							},"-",{
								text: "������Ϣ",
								cls: "menuItem",
								handler:function(){
									parent.compidTmp = record.get('compid')+"comp";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'comp',
				            text:'�������Ϣ',
				            srcurl:'ncViewer.lanComp'
			        		},0);
								}
							},{
								text: "Ӳ����Ϣ",
								cls: "menuItem",
								handler:function(){
									parent.compidTmp = record.get('compid')+"comp";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'comp',
				            text:'�������Ϣ',
				            srcurl:'ncViewer.lanComp'
			        		},1);
								}
							},{
								text: "�����Ϣ",
								cls: "menuItem",
								handler:function(){
									parent.compidTmp = record.get('compid')+"comp";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'comp',
				            text:'�������Ϣ',
				            srcurl:'ncViewer.lanComp'
			        		},2);
								}
							},{
								text: "������Ϣ",
								cls: "menuItem",
								handler:function(){
									parent.compidTmp = record.get('compid')+"comp";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'comp',
				            text:'�������Ϣ',
				            srcurl:'ncViewer.lanComp'
			        		},3);
								}
							},{
								text: "������Ϣ",
								cls: "menuItem",
								handler:function(){
									parent.compidTmp = record.get('compid')+"comp";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'comp',
				            text:'�������Ϣ',
				            srcurl:'ncViewer.lanComp'
			        		},4);
								}
							},{
								text: "����������Ϣ",
								cls: "menuItem",
								handler:function(){
									parent.compidTmp = record.get('compid')+"comp";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'comp',
				            text:'�������Ϣ',
				            srcurl:'ncViewer.lanComp'
			        		},5);
								}
							},"-",{
								xtype: 'label',
								text: '�鿴��־��Ϣ',
								padding: '5 5 5 30',
								cls: "menuTitle"
							},"-",{
								text: '������־',
								cls: "menuItem",
								handler: function(){
									parent.compidTmp = record.get('compid')+"rzck";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'rzck',
				            text:'��־�鿴',
				            srcurl:'ncViewer.lanRzck'
			        		},0);
								}
							},{
								text: '��Ļ��־',
								cls: "menuItem",
								handler: function(){
									parent.compidTmp = record.get('compid')+"rzck";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'rzck',
				            text:'��־�鿴',
				            srcurl:'ncViewer.lanRzck'
			        		},1);
								}
							},{
								text: '��վ������־',
								cls: "menuItem",
								handler: function(){
									parent.compidTmp = record.get('compid')+"rzck";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'rzck',
				            text:'��־�鿴',
				            srcurl:'ncViewer.lanRzck'
			        		},2);
								}
							},{
								text: '����ʹ����־',
								cls: "menuItem",
								handler: function(){
									parent.compidTmp = record.get('compid')+"rzck";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'rzck',
				            text:'��־�鿴',
				            srcurl:'ncViewer.lanRzck'
			        		},3);
								}
							},{
								text: '����ʹ����־',
								cls: "menuItem",
								handler: function(){
									parent.compidTmp = record.get('compid')+"rzck";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'rzck',
				            text:'��־�鿴',
				            srcurl:'ncViewer.lanRzck'
			        		},4);
								}
							},{
								text: '�ļ�������־',
								cls: "menuItem",
								handler: function(){
									parent.compidTmp = record.get('compid')+"rzck";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'rzck',
				            text:'��־�鿴',
				            srcurl:'ncViewer.lanRzck'
			        		},5);
								}
							},{
								text: '�澯��־',
								cls: "menuItem",
								handler: function(){
									parent.compidTmp = record.get('compid')+"rzck";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'rzck',
				            text:'��־�鿴',
				            srcurl:'ncViewer.lanRzck'
			        		},6);
								}
							},"-",{
								text: '�ػ�',
								cls: "menuItem",
								handler: function(){
									parent.shutDown(record.get('compid'));
								}
							},{
								text: '����',
								cls: "menuItem",
								handler: function(){
									parent.restart(record.get('compid'));
								}
							},{
								text: 'ж�ؿͻ���',
								handler: function(){
									parent.uninstal(record.get('compid'));
								}
							}]
						}).showAt(event.getXY());
						}
        	}
   			}
   
   
        });

        this.callParent(arguments);
    }
});