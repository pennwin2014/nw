/**
 �������tab
 */
 
Ext.define('ncViewer.lanProcy',{
    extend: 'Ext.panel.Panel',
layout: {type: 'hbox',
	padding: '0 0 0 0'

	},
	border:false,
    initComponent : function(){
        Ext.apply(this,{
            id: 'procy-panel',
            items: [this.createTree(),this.createCenter()]
        });
        this.callParent(arguments);
    },
   createTree:function(){ 
    	
     Ext.define('treedata', {
        extend: 'Ext.data.Model',
        fields: [
            {name: 'id',     type: 'string'},
            {name: 'text',type:'string'},
            {name: 'pname', type: 'string'},
            {name: 'status',type:'string'}
        ]
    });

    	  	
    	  var store_tree = Ext.create('Ext.data.TreeStore', {
    	  model: 'treedata',
        proxy: {
            type: 'ajax',
            url: '/pronline/Msg?FunName@lanTreeProcy'
        },
        root: {
            text: '������',
            id: '',
            expanded: true
        },
        folderSort: true, 
        sorters: [{
            property: 'text',
            direction: 'ASC'
        }]
     });
    
	Ext.define('EimData2',{
		extend:'Ext.data.Model',
		fields:['id']
	});
       var store_procyref = Ext.create('Ext.data.Store', {
        model: 'EimData2',
        remoteSort: true,
		 autoLoad:true, 
        proxy: {
            type: 'ajax',
			url:'/pronline/Msg?FunName@lan_Procy_count_procyref',
            reader: {
                type:'json',
                root: 'data',
                totalProperty: 'totalCount'
            }, 
			  simpleSortMode: true
			}     
    });
	
	var store_procyobj = Ext.create('Ext.data.Store', {
        model: 'EimData2',
        remoteSort: true,
		 autoLoad:true, 
        proxy: {
            type: 'ajax',
			url:'/pronline/Msg?FunName@lan_Procy_count_procyobj',
            reader: {
                type:'json',
                root: 'data',
                totalProperty: 'totalCount'
            }, 
			  simpleSortMode: true
			}     
    });
    

    
    this.tree_procy = Ext.create('Ext.tree.Panel', {
        store: store_tree,

        id:'tree_procy',
        height: 950,
        width: 230,
       region: 'west',
        containerScroll: true,
        useArrows: false,     
          dockedItems: [
          {
            xtype: 'toolbar',
            items: [{
             text: '����������',
                iconCls:'add',
                 handler: function(){
                 	opedit('');
  

                }               
            },{
            	text:'ɾ��',
				
            	iconCls:'remove',
            	handler:function(){
            	
            		var records=Ext.getCmp('tree_procy').getView().getChecked();
            		var temps='';
            		var iNum='';
           		Ext.Array.each(records,function(rec){
           			     if(iNum==0){
                    temps=rec.get('id');
                    }
                    else{
                    temps=temps+','+rec.get('id');
                    }
                    iNum++;
					
				  }
           		  )
				//	alert(temps);
					store_procyref.load({params:{sid:temps}});
					store_procyobj.load({params:{sid:temps}});
					alert("ѡ��ɾ�� "+iNum+" ������");
					alert("ѡ����Ա������������ô���Ϊ: " + store_procyref.getAt(0).get('id')
						+"\nѡ����Ա��¶�����Ϊ: " + store_procyobj.getAt(0).get('id'));
					//alert("ѡ����Ա��¶�����Ϊ: " + store_procyobj.getAt(0).get('id'));
					
           		  if(temps==''){
           		  	alert('��ѡ��ɾ���Ĳ���');
           		  	return;
           		  }
           		  if(confirm('�����Ҫִ��ɾ��������')){  
           		  	store_tree.load({params: {del: "del",selsid:temps}});
           		  }
           		  
            	}
            	
            },{
            	 text: '�·�����',
            	 handler: function(){
            	 	ncapp.showTabs({
						   		text: '�����·�',
                	itemid:'clxf',
                	srcurl:'ncViewer.lanProcyXf'
					      });
            	 }
            }]
        }]
    
        
        
        
        
        
      });
      function opedit(rcd){
     
  
							    var constatsCombo = Ext.create('Ext.form.field.ComboBox', {
							       								   fieldLabel: '����״̬',
															        valueField: 'groupid',								       
															        displayField: 'groupname',
															         name:'constatus',      
															        value:'0',
															        allowBlank: true,
															        queryMode: 'local',
															        typeAhead: true,
															         store: Ext.create('Ext.data.Store', 
							                               	 	{
							                                    		fields : ['groupname', 'groupid'],
							                                    		data   : 
							                                    		[
							                                        		{groupname : '������Ч',  groupid: '0'},
							                                        		{groupname : '������Ч',  groupid: '1'}
							                                        		
							                                    		]
							                                	})

							    });

  
       	            var winadd;
                    var addform=Ext.create('Ext.FormPanel', {         
						          frame: true,
			                fieldDefaults: {
					          	labelWidth: 85  ,                      
			              	xtype: 'textfield' 
					        	 },
						           bodyPadding: 10,
                      items: [                  								     								   
                      {                        
                        fieldLabel:'��������',
                        xtype: 'textfield',
                        name:'text'
                      },
                      constatsCombo,
                      {
                    	 xtype: 'hidden', 
											 name: 'id'
											
                      },
                      {
                        xtype: 'button',
                         anchor: 'right',
                         style : 'margin-left:80px',
                         text:'��&nbsp;&nbsp;��',
                         handler: function(){
                         	var keyname=addform.getForm().getValues().text;
                         	var sid=addform.getForm().getValues().id;
                         	var cstatus=addform.getForm().getValues().constatus;
                       //  	alert("sid = "+sid);
                          if(keyname==''){
                         	  alert('����д��������');
                          }
                         else{                       
                           store_tree.load({params: {update: "update",keyname:keyname,sid:sid,constatus:cstatus}});
                           Ext.getCmp('winadd').close();
                         }
                         
                         }
                       
                      },
                      {
                    	xtype: 'button',
                    	 style : 'margin-left:10px',
                    	text:'��&nbsp;&nbsp;��',
                    	handler: function(){
          //          		alert(addform.getForm().getValues().id);
                    		Ext.getCmp('winadd').close();
                    	}
                    }
                    
                    
                    ]
                   });
                                    
       //           addform.loadRecord(rcd);
       if(rcd!=''){
       addform.getForm().findField('id').setValue(rcd.get('id'));
       addform.getForm().findField('constatus').setValue(rcd.get('status'));
       addform.getForm().findField('text').setValue(rcd.get('pname'));
     }
                   winadd=new Ext.Window ({
                             id:"winadd",
                             title:"����/�޸�������",
                             x:250,
                             y:130,
                             width:300,
                             height:160,
                             bodyPadding: 10,
                             modal:true,
                             resizable:true,
                             closable : true,
                //             draggable:true,
                          
                             items: addform
                                                
                  }
            );  

           winadd.show();

   
   
   
   
          
      }
   //�������ӵ����˫���¼�
    Ext.getCmp('tree_procy').on({             
	       'itemclick' : function(view, rcd, item, idx, event, eOpts) {   
	         
	             var active = this.tab_procy.getActiveTab();
	  //           alert(rcd.get('text'));
	//             active.tab.setText('�ؼ��ֲ�ѯ');
	var aaa;
	if(rcd.get('text')!='������'){
	  aaa="�����ԣ�"+rcd.get('text');
  }
  else{
  	aaa='���ȵ������������ѡȡ������...';
  }
  this.tab_procy.setTitle(aaa);
  pid=rcd.get('id');	
	var tabframe="procy_"+active.id;
   if((eval(tabframe).store)){

   		eval(tabframe).store.currentPage=1;
   	//	if(eval(tabframe).Ext.getCmp('keyword')){
    //			eval(tabframe).Ext.getCmp('keyword').setValue('');
    //  }

  }

	eval(tabframe).reflash();
	       	  },       
	     //Ŀ¼��˫�����¼�          
	       'itemdblclick' : function(view, rcd, item, idx, event, eOpts) {
	        opedit(rcd);
	         },              
	         scope : this        });
    	return this.tree_procy;
    },
    createCenter:function(){
    	swidth=screen.width-470;
    	pid='';
    	 this.tab_procy = Ext.createWidget('tabpanel', {  
    	 	title:'���ȵ������������ѡȡ������...',
        width:swidth,
        id: 'center_procy',
        region: 'center',
        margins: '0 0 0 0',
        forceFit: true,
   //     scrollOffset: 0,
        activeTab: 0,        
         defaults :{
 
            bodyPadding: 0
 
        },
         items: [{
            title: '�ļ�����', 
            id:'file',           
            html: '<iframe  id="procy_file" name="procy_file" src="/v8/lanprocy/lan_procy_file.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },
        {
            title: '�������',
            id:'dev',            
            html: '<iframe  id="procy_dev" name="procy_dev" src="/v8/lanprocy/lan_procy_device.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '�Ƿ���������',
            id:'link',            
            html: '<iframe  id="procy_link" name="procy_link" src="/v8/lanprocy/lan_procy_link.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: 'Ӧ�ó������',
            id:'pgtype',           
            html: '<iframe  id="procy_pgtype" name="procy_pgtype" src="/v8/lanprocy/lan_procy_pgtype.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '���̿���',
            id:'pname',    
            html: '<iframe  id="procy_pname" name="procy_pname" src="/v8/lanprocy/lan_procy_pname.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '�������',  
            id:'net',          
            html: '<iframe  id="procy_net" name="procy_net" src="/v8/lanprocy/lan_procy_net.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '��ַ����',  
            id:'url',          
            html: '<iframe  id="procy_url" name="procy_url" src="/v8/lanprocy/lan_procy_url.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '��־���Ͳ���', 
            id:'screen',           
            html: '<iframe  id="procy_screen" name="procy_screen" src="/v8/lanprocy/lan_procy_screen.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: 'ʹ��Ԥ�������', 
            id:'preid',           
            html: '<iframe  id="procy_preid" name="procy_preid" src="/v8/lanprocy/lan_procy_preid.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        }
        ]
      });
      
      //Ȩ�޴���
      var str = right_fun;
      
      this.tab_procy.items.each(function(item){
      	//console.info(str,",",item.getId(),",",str.indexOf(item.getId()));
      	if(str.indexOf(item.getId()+",")== -1){
      		item.destroy();
      	}
      });

      return this.tab_procy;
    }
    	
})