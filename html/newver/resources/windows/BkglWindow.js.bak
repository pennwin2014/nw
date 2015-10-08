
Ext.define('MyDesktop.BkglWindow', {
    extend: 'Ext.ux.desktop.Module',

   

    id:'bkgl-win',

    init : function(){
        this.launcher = {
            text: '布控管理',
            iconCls:'icon-grid'
        };
    },

    createWindow : function(){
        var desktop = this.app.getDesktop();
        var win = desktop.getWindow('bkgl-win');
         var htmlvar='<iframe src="/v4/ncs_case_lab.htm" style="width:1020px;height:1000px;display:block;" FrameBorder=0 scrolling="No">';
        if(!win){
            win = desktop.createWindow({
                id: 'bkgl-win',
                title:'布控管理',
                width:1024,
                height:700,
                autoScroll: true,
                iconCls: 'icon-grid',
                animCollapse:false,
                constrainHeader:true,
    //            layout: 'fit',
                 html:htmlvar

            });
        }
        return win;
    }
});

