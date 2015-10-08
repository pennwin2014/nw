/**
 定义部门树
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
		            text: '全部',
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
		            if(records && records[0] && records[0].get('text')=='全部'){
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
        {"groupid":"","groupname":"状态2"},
        {"groupid":"1","groupname":"在线"},
        {"groupid":"2","groupname":"离线"}
     
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
                    if(vkeyword=='计算机名/IP/MAC'){
                    	vkeyword='';
                    }
		//	          	var vstate=Ext.getCmp('compstate').value;
			          	  store_comp.load({params: {keyword:vkeyword}});
			          }
			  }
    });
    	
   



    Ext.apply(this, {
 //       title: '部门电脑',
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
               emptyText:'计算机名/IP/MAC',
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
        	dxtitle='部门：'+rcd.get('text');
        	dxtitle_c='部门：'+rcd.get('ctext');
          }
          if(compid!=''){
          	dxtitle='计算机：'+rcd.get('text');
          	dxtitle_c='计算机：'+rcd.get('ctext');
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
        			if(record.get('text').indexOf('离线')>0){
        				cmpStatus = true;
        			}else{
        				cmpStatus = false;
        			}
	        		Ext.create('Ext.menu.Menu', {
							floating:	true,
							items: [{
								xtype: 'label',
								text: '查看计算机信息',
								padding: '5 5 5 30',
								cls: "menuTitle"
							},"-",{
								text: "基本信息",
								cls: "menuItem",
								handler:function(){
									parent.compidTmp = record.get('compid')+"comp";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'comp',
				            text:'计算机信息',
				            srcurl:'ncViewer.lanComp'
			        		},0);
								}
							},{
								text: "硬件信息",
								cls: "menuItem",
								handler:function(){
									parent.compidTmp = record.get('compid')+"comp";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'comp',
				            text:'计算机信息',
				            srcurl:'ncViewer.lanComp'
			        		},1);
								}
							},{
								text: "软件信息",
								cls: "menuItem",
								handler:function(){
									parent.compidTmp = record.get('compid')+"comp";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'comp',
				            text:'计算机信息',
				            srcurl:'ncViewer.lanComp'
			        		},2);
								}
							},{
								text: "服务信息",
								cls: "menuItem",
								handler:function(){
									parent.compidTmp = record.get('compid')+"comp";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'comp',
				            text:'计算机信息',
				            srcurl:'ncViewer.lanComp'
			        		},3);
								}
							},{
								text: "磁盘信息",
								cls: "menuItem",
								handler:function(){
									parent.compidTmp = record.get('compid')+"comp";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'comp',
				            text:'计算机信息',
				            srcurl:'ncViewer.lanComp'
			        		},4);
								}
							},{
								text: "网络连接信息",
								cls: "menuItem",
								handler:function(){
									parent.compidTmp = record.get('compid')+"comp";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'comp',
				            text:'计算机信息',
				            srcurl:'ncViewer.lanComp'
			        		},5);
								}
							},"-",{
								xtype: 'label',
								text: '查看日志信息',
								padding: '5 5 5 30',
								cls: "menuTitle"
							},"-",{
								text: '聊天日志',
								cls: "menuItem",
								handler: function(){
									parent.compidTmp = record.get('compid')+"rzck";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'rzck',
				            text:'日志查看',
				            srcurl:'ncViewer.lanRzck'
			        		},0);
								}
							},{
								text: '屏幕日志',
								cls: "menuItem",
								handler: function(){
									parent.compidTmp = record.get('compid')+"rzck";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'rzck',
				            text:'日志查看',
				            srcurl:'ncViewer.lanRzck'
			        		},1);
								}
							},{
								text: '网站访问日志',
								cls: "menuItem",
								handler: function(){
									parent.compidTmp = record.get('compid')+"rzck";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'rzck',
				            text:'日志查看',
				            srcurl:'ncViewer.lanRzck'
			        		},2);
								}
							},{
								text: '进程使用日志',
								cls: "menuItem",
								handler: function(){
									parent.compidTmp = record.get('compid')+"rzck";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'rzck',
				            text:'日志查看',
				            srcurl:'ncViewer.lanRzck'
			        		},3);
								}
							},{
								text: '外设使用日志',
								cls: "menuItem",
								handler: function(){
									parent.compidTmp = record.get('compid')+"rzck";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'rzck',
				            text:'日志查看',
				            srcurl:'ncViewer.lanRzck'
			        		},4);
								}
							},{
								text: '文件操作日志',
								cls: "menuItem",
								handler: function(){
									parent.compidTmp = record.get('compid')+"rzck";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'rzck',
				            text:'日志查看',
				            srcurl:'ncViewer.lanRzck'
			        		},5);
								}
							},{
								text: '告警日志',
								cls: "menuItem",
								handler: function(){
									parent.compidTmp = record.get('compid')+"rzck";
									parent.compNameTmp = record.get('compname');
									parent.ncapp.showTabs({
				            itemid:'rzck',
				            text:'日志查看',
				            srcurl:'ncViewer.lanRzck'
			        		},6);
								}
							},"-",{
								text: '关机',
								cls: "menuItem",
								handler: function(){
									parent.shutDown(record.get('compid'));
								}
							},{
								text: '重启',
								cls: "menuItem",
								handler: function(){
									parent.restart(record.get('compid'));
								}
							},{
								text: '卸载客户端',
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