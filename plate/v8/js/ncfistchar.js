/*
首页的有关图表
*/

Ext.define('ncViewer.charPoral', {
    extend: 'Ext.panel.Panel',
    alias: 'widget.ncCharPoral',
    initComponent: function(){

        Ext.apply(this, {
            id: 'yxfx',
            title:'运行状态',
             margin: '0 0 0 0',
             scrollOffset: 0,
             autoScroll:true,
             tbar:[{
             	text:'选择显示模块',
             	 xtype:'splitbutton',
             	 iconCls:'add'
            },'-',{
             	text:'恢复默认模块',
             	iconCls:'remove'
            }],
            layout: {              
                padding: '0 0 0 0'
            },
            items: [           
            {
            	height:300,
              layout: {type: 'hbox', align: 'stretch'},
              margin: '0 0 0 0',
               border: false,
               bodyStyle: 'background-color: transparent',
            	 items:[{
            	  flex:1,
            	  title:'系统资源信息',
                id: 'app-header',              
                html: '1显示首页内容'
            },
            {
            	height:300,
            	title:'网络接口实时速率图',
            	flex:1,
              layout: {type: 'vbox', align: 'stretch'},
              margin: '0 0 0 0',
               border: true,
               bodyStyle: 'background-color: transparent',
            	 items:[this.createNetflowForm(),this.createNcNetflowChar()]
            	}
            
            ]
           }
           
           ,
            {
            	height:300,
             layout: {type: 'hbox', align: 'stretch'},
              margin: '0 0 0 0',
                border: false,
               bodyStyle: 'background-color: transparent',
            	 items:[{
            	  flex:1,
            	  title:'3aaafasdf',
                id: 'app-header2',
             
                html: '1显示首页内容'
            },
            {
            	title:'4aaafasdf',
                id: 'app-header3',
               flex:1,
               
                html: '显示首页内容'
              }
            ]
           }
           
           
           ]
                    
               
           
        });
        this.callParent(arguments);
    },
    createNcNetflowChar:function(){  
         var store_netflow=Ext.create('Ext.data.Store', 
       	 {
        		pageSize: 40,
        		id:"store_netflow",
 //       		model: 'EimData',   
          fields:['dxname','xname',[#SLPB#][#dh#]{name:'[#dname#]',mapping:'[#dname_a#]',type:'int'}[#SLPE#]],
        		autoLoad:true,
        		remoteSort: true,
        		proxy: 
        		{
            			type: 'ajax',
            			url: '/pronline/Msg?FunName@ncShowNetflowAjax_v8',
            			reader: {
                			type:'json',
                			root: 'eimdata',
                			totalProperty: 'totalCount'
            			},         
            			simpleSortMode: true
        		},
        		sorters: 
        		[{
            			property: 'stime',
            			direction: 'DESC'
        		}]
    		});
    
  
    
		   var taskManager=new Ext.util.TaskRunner();
		   var task={
		   	 run:function(){
		    
		        var store=Ext.StoreManager.lookup("store_netflow");
            store.load(); 
            var fid=Ext.getCmp('showflow');
  //          fid.setText('fsafsdfds');
		   	},interval:10000
		  };
		  taskManager.start(task);

     var Chart1 = Ext.create('Ext.chart.Chart', {
            xtype: 'chart',
            flex:1,
           margin: '5 20 10 10',
            style: 'background:#fff',

            animate: true,
            store: store_netflow,
            shadow: false,
            theme: 'Category1',
            legend: {
                position: 'bottom'
            },
            axes: [
            {
                type: 'Numeric',
                minimum: 0,
 //          constrain:true,
            maximum: 200,
                position: 'left',
                fields: [[#SLPB#][#dh#]'[#dname#]'[#SLPE#]],
              
                grid: { odd: {opacity: 1,fill: '#fff',stroke: '#bbb','stroke-width': 0.5},               
                       even: {opacity: 1,fill: '#fff',stroke: '#bbb','stroke-width': 0.5}
                }
              },
                          {
                type: 'Numeric',
               minimum: 0,
 //          constrain:true,
               maximum: 200,
                position: 'right',
                fields: [[#SLPB#][#dh#]'[#dname#]'[#SLPE#]],
              
                grid: { odd: {opacity: 1,fill: '#fff',stroke: '#bbb','stroke-width': 0.5},               
                       even: {opacity: 1,fill: '#fff',stroke: '#bbb','stroke-width': 0.5}
                }
              }
              , 
              
              
              {
                type: 'Category',
                position: 'bottom',
                fields:['dxname'],  
                label: {font: '11px Arial',rotate: {degrees: 0}}            
            }],
            series: [
           [#SLPB#] [#dh#]
            {
                type: 'line',
                    style: {
                        'stroke-width': 1
   //                     stroke: 'rgb(17, 95, 166)'
                    },
                axis: 'left',
                xField: 'xname',
                yField: '[#dname#]',
                 markerConfig: {
                    type: 'cross',
                    size: 0,
                    radius: 0,
                    'stroke-width': 0
                },
                smooth:true
            }
            [#SLPE#]
           ]
        
        });
       return Chart1;       
    },
    createNetflowForm:function(){  
    	   var panel =  Ext.create('Ext.FormPanel', 
           {         
						frame: false,
						border:true,

						margin: ' 0 0 0',
						layout: 'hbox',					
						bodyPadding: 0,
            items: [{      
            	            margin: '8 0 0 0',     
                        	xtype:'label',
                        	width:150,
                        	height:'20',
                          id:'showflow',
                          html:'&nbsp;&nbsp;单位：([#unit#])&nbsp;'                        	
                    	},
                    	{      
                    		  width:350,
                    		
            	            margin: '8 0 0 0',     
                        	xtype:'label',                       	
                        	height:'20',
                          id:'showflow1',
                          html:'<span style="margin-right:100px"><font color=red>上行:</font>100kbps &nbsp;<font color=red>下行:</font>30kbps&nbsp;</span>'                        	
                    	}
                    ]
             }
          );
                    	

    	return panel;
    	
    	
    	
    }
    
});


            
       
