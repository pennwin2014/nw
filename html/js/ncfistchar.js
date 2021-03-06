/*
首页的有关图表
*/

Ext.define('ncViewer.charPoral', {
    extend: 'Ext.panel.Panel',
    alias: 'widget.ncCharPoral',
//    uses: ['Ext.app.PortalPanel', 'Ext.app.PortalColumn', 'Ext.app.GridPortlet', 'Ext.app.ChartPortlet'],

    initComponent: function(){

        Ext.apply(this, {
            id: 'app-viewport',
            title:'运行状态',
            
             margin: '0 0 0 0',
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
            	height: 350,
              layout: {type: 'hbox', align: 'stretch'},
              margin: '0 0 0 0',
               border: false,
               bodyStyle: 'background-color: transparent',
            	 items:[{
            	  flex:1,
            	  title:'aaafasdf',
                id: 'app-header',
   //             xtype: 'box',
                
              
                html: '1显示首页内容'
            },

            {
            	height: 350,
            	title:'网络接口实时速率图',
            	flex:1,
              layout: {type: 'hbox', align: 'stretch'},
              margin: '0 0 0 0',
               border: true,
               bodyStyle: 'background-color: transparent',
            	 items:[this.createNcNetflowChar()]
            	}
            
            ]
           }
           
           ,
            {
            	height: 350,
             layout: {type: 'hbox', align: 'stretch'},
              margin: '0 0 0 0',
                border: false,
               bodyStyle: 'background-color: transparent',
            	 items:[{
            	  flex:1,
            	  title:'3aaafasdf',
                id: 'app-header2',
   //             xtype: 'box',
                
              
                html: '1显示首页内容'
            },
            {
            	title:'4aaafasdf',
                id: 'app-header3',
       //         xtype: 'box',
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
            fields: ['name', 'xname','data1', 'data2', 'data3'],
        		autoLoad:true,
        		remoteSort: true,
        		proxy: 
        		{
            			type: 'ajax',
            			url: '/pronline/Msg?FunName@ncsUserlogStatByday_v4',
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
		   //     store.remove(store.getAt(0));
		   //     store.add({"name":v,"xname":v,"data1":v,"data2":v,"data3":v});
		     
		   	},interval:3000
		  };
		  taskManager.start(task);
// store1.loadData(data1);

    //create a bar series to be at the top of the panel.
    var Chart1 = Ext.create('Ext.chart.Chart', {
            xtype: 'chart',
            flex:1,
           margin: '20 20 10 10',
            style: 'background:#fff',

            animate: true,
            store: store_netflow,
            shadow: false,
  //          theme: 'Category1',
            legend: {
                position: 'bottom'
            },
            axes: [{
                type: 'Numeric',
                minimum: 0,
                position: 'left',
                fields: ['data1', 'data2', 'data3'],
     //           title: '网络接口速率(M/s)',
                minorTickSteps: 1,
              
                grid: {
                    odd: {
                        opacity: 1,
                        fill: '#fff',
                        stroke: '#bbb',
                        'stroke-width': 0.5
                    }  ,               
                     even: {
                        opacity: 1,
                        fill: '#fff',
                        stroke: '#bbb',
                        'stroke-width': 0.5
                    }
                }
            }, {
                type: 'Category',
                position: 'bottom',
                fields:['xname'],
                steps:3,
                
                
                 label: {
                
                font: '11px Arial',
                rotate: {
                    degrees: 0
                }
              }
            
 //               title: 'Month of the Year'
            }],
            series: [{
                type: 'line',
  //              highlight: {
  //                  size: 1,
 //                   radius: 1
  //              },
                    style: {
                        'stroke-width': 1,
                        stroke: 'rgb(17, 95, 166)'

                    },
                axis: 'left',
                xField: 'name',
                yField: 'data1',
 //               smooth:true,
                markerConfig: {
                    type: 'cross',
                    size: 0,
                    radius: 0,
                    'stroke-width': 0
                }
            }, {
                type: 'line',
                 
                highlight: {
                    size: 1,
                    radius: 1
                },
                axis: 'left',
//                smooth: true,
                xField: 'name',
                yField: 'data2',
                markerConfig: {
                    type: 'circle',
                    size: 0,
                    radius: 0,
                    'stroke-width': 0
                }
            }, {
                type: 'line',
                
                highlight: {
                    size: 1,
                    radius: 1
                },
                axis: 'left',
 //               smooth: true,
                fill: false,
                xField: 'name',
                yField: 'data3',
                markerConfig: {
                    type: 'circle',
                    size: 1,
                    radius: 1,
                    'stroke-width': 1
                }
            }]
        
        });
       return Chart1;
       
    }
    
});


            
       
