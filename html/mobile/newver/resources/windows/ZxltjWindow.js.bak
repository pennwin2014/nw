
Ext.define('MyDesktop.GdrzWindow', {
    extend: 'Ext.ux.desktop.Module',

   

    id:'gdrz-win',

    init : function(){
        this.launcher = {
            text: '归档日志',
            iconCls:'icon-grid'
        };
    },

    createWindow : function(){
        var desktop = this.app.getDesktop();
        var win = desktop.getWindow('gdrz-win');
         var htmlvar='<iframe src="/v4/windows/main_gdrz.htm" style="width:1350px;height:950px;display:block;" FrameBorder=0 scrolling="yes">';
        if(!win){
            win = desktop.createWindow({
                id: 'dwgl-win',
                title:'归档日志',
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

