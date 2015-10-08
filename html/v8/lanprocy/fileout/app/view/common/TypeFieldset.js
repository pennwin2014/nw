Ext.define('Fileout.view.common.TypeFieldset',{
  extend: 'Ext.form.FieldSet',
  alias : 'widget.typefieldset',  
  width: 240,
  height: 300,
  comboName:null,
  disable:false,
  comboLabel: null,
  comboStore: null,
  columns: null,
  gridStore: null,
  item1: null,
  item2: null,
  item3: null,
  
  initComponent: function(){
  	this.items = [{
  	  xtype: 'grid',
  	  width: 215,
  	  height: 140,
  	  tbar: [{
  	    xtype: 'combo',
  	    action: 'combo',
  	    fieldLabel: this.comboLabel,
      	    labelWidth: 58,
      	    width: 137,
            store: this.comboStore,
            queryMode: 'local',
            displayField: 'text',
            name:this.comboName,
            valueField: 'id',
      	    triggerAction: 'all'
  	  },{
  	    text: '增加',
  	    action: 'edit'  
  	  },{
  	    text: '删除',
  	    action: 'delete'  
  	  }],
  	  rbar:[{
  	    text: '增加',
  	    action: 'add'
  	  },{
  	    text: '修改',
  	    disabled: this.disable,
  	    action: 'mod'
  	  },{
  	    text: '删除',
  	    action: 'del'
  	  },{
  	    text: '删除全部',
  	    action: 'delall'
  	  }],
  	  store: this.gridStore,
  	  columns: this.columns
  	},this.item1,this.item2,this.item3];
     	
  	this.callParent(arguments);
  }
});