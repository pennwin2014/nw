
Ext.define('MyDesktop.MggzWindow', {
    extend: 'Ext.ux.desktop.Module',

   

    id:'mggz-win',

    init : function(){
        this.launcher = {
            text: '���й���',
            iconCls:'icon-grid'
        };
    },

    createWindow : function(){
        var desktop = this.app.getDesktop();
        var win = desktop.getWindow('mggz-win');
        var htmlvar='<iframe src="/pronline/Msg?FunName@ncsWebReplace&plate@windows/main_frm.htm&title@���й���" style="width:1350px;height:950px;display:block;" FrameBorder=0 scrolling="yes">';
        if(!win){
            win = desktop.createWindow({
                id: 'mggz-win',
                title:'���й���',
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

