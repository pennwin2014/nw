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
            {name: 'text',     type: 'string'},
            {name: 'id',     type: 'string'},
            {name: 'compid', type: 'string'},
            {name:'ctext',   type:'string'}

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
            id: '',
            expanded: true
        },
        folderSort: true,
        sorters: [{
 //           property: 'text',
//            direction: 'ASC'
        }]
    });
    	store_comp.on('beforeload', function (store, options) {

       var new_params = { state: Ext.getCmp('compstate').getValue() };
        Ext.apply(store.proxy.extraParams, new_params);
       });
    
        var compstate = [
        {"groupid":"","groupname":"状态"},
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
//  	  id:'tree_comp',
        store: store_comp,
        autoScroll: true,
         border:false,
      forceFit: true,
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
               value:'计算机名/IP/MAC',
               enableKeyEvents:true,
               listeners:{
                 'focus':function(){
                 if(this.value=='计算机名/IP/MAC'){                
                    this.setValue('');
                  }
                 },
         
                 'keydown' : function(i,e){
                var aae=e.getKey() ; 
                if(aae==13){
                    var aa=Ext.getCmp('keyword').value;
                    if(aa!='计算机名/IP/MAC'){
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
                handler:function(){
                	  var aa=Ext.getCmp('keyword').value;
                    if(aa!='计算机名/IP/MAC'){
                        var vkeyword=Ext.getCmp('keyword').value;			         
						          	store_comp.load({params: {keyword:vkeyword}});
						          }
   
                }
 
                
            }
             
             
             
             ]}]
              ,        
         listeners:{itemclick:function(view, rcd, item, idx, event, eOpts){
        	groupid=rcd.get('id');        
        	compid=rcd.get('compid');
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
          
        	var tabs_center=Ext.getCmp("layout_center");
        	var active = tabs_center.getActiveTab();
    //    	var tabframe="procy_"+active.id;
       // 	var tabs_if=Ext.getCmp(active.id);
        	
        	var active_id='lay_'+active.id;
        	var tabs_if=Ext.getCmp(active_id).getActiveTab();
        	var tabframe=active_id+'_'+tabs_if.id;
      // 	alert(tabframe);
     //   	eval(tabframe).store.currentPage=1;
          eval(tabframe).SetPage();
        	eval(tabframe).reflash();
       //  aaaa=Ext.getCmp('layout_center');
        
  //      clicknode(groupid,compid);
         }
        } 
   
   
   
   
   
        });

        this.callParent(arguments);
    }
});