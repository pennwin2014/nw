Ext.define('LanProcy.view.common.TypeFieldset',{
  extend: 'Ext.form.FieldSet',
  alias : 'widget.typefieldset',
  
  gid: null,
  combo: null,
  columns: null,
  store: null,
  item1: null,
  item2: null,
  item3: null,
  
  initComponent: function() { 	
  	this.items = [{
  	  xtype: 'grid',
  	  id: this.gid,
  	  width: 400,
  	  height: 200,
  	  tbar: [this.combo,{
  	    text: '�༭',
  	    action: 'edit'  
  	  }],
  	  rbar:[{
  	    text: '����',
  	    action: 'add'
  	  },{
  	    text: '�޸�',
  	    action: 'mod'
  	  },{
  	    text: 'ɾ��',
  	    action: 'del'
  	  },{
  	    text: 'ɾ��ȫ��',
  	    action: 'delall'
  	  }],
  	  store: this.store,
  	  columns: this.columns
  	},this.item1,this.item2,this.item3];
     	
  	this.callParent(arguments);
  }
});