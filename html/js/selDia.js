//����-vessel-return 1.ID;2.��������3.Ӣ����
//�ͻ�-client-return 1.ID;2.��������3.Ӣ����;4.��������+���ĵ�ַ+�绰+����;5.Ӣ������+Ӣ�ĵ�ַ+�绰+����
//�ֹ���-supplier-return 1.ID;2.��������3.Ӣ����;4.��������+���ĵ�ַ+�绰+����;5.Ӣ������+Ӣ�ĵ�ַ+�绰+����
//��λ-unit-return 1.ID;2.��������3.Ӣ����
//���õȼ�-clevel-return 1.ID;2.��������3.Ӣ����
//��ҵ-industry-return 1.ID;2.��������3.Ӣ����
//�ͻ���Դ-csource-return 1.ID;2.��������3.Ӣ����
//���ʽ-pmode-return 1.ID;2.��������3.Ӣ����
//��ʱ����-otreason-return 1.ID;2.��������3.Ӣ����
//��������-otreason-return 1.ID;2.��������3.Ӣ����
//��������-otreason-return 1.ID;2.��������3.Ӣ����
//�ɱ���Ŀ-cost_item-return 1.ID;2.��������3.Ӣ����
//������Ŀ-server_item-return 1.ID;2.��������3.Ӣ����
//������Ŀ-ope_item-return 1.ID;2.��������3.Ӣ����
//��֤��Ŀ-doc_item-return 1.ID;2.��������3.Ӣ����
//������-open_bank-return 1.ID;2.��������3.Ӣ����
//ҵ����-ope_no-return 1.ID;2.ҵ���ţ�3.ί��ʱ�䣻

function selDia(selName)
{
	var wb;
	var objEva=new Array();
	for(var i=1;i<=arguments.length/2;i++)
	{
		switch(arguments[2*i])
		{
			case 'id':
				if(typeof(eval('document.all.'+arguments[2*i-1]))=="object")
					objEva[0]=eval('document.all.'+arguments[2*i-1]);
				break;
			case 'cn':
				if(typeof(eval('document.all.'+arguments[2*i-1]))=="object")
					objEva[1]=eval('document.all.'+arguments[2*i-1]);
				break;
			case 'en':
				if(typeof(eval('document.all.'+arguments[2*i-1]))=="object")
					objEva[2]=eval('document.all.'+arguments[2*i-1]);
				break;
			case 'cd':
				if(typeof(eval('document.all.'+arguments[2*i-1]))=="object")
					objEva[3]=eval('document.all.'+arguments[2*i-1]);
				break;
			case 'ed':
				if(typeof(eval('document.all.'+arguments[2*i-1]))=="object")
					objEva[4]=eval('document.all.'+arguments[2*i-1]);
				break;
		}
	}
	if(selName=="vessel")
	{
		wb="/Wss/Wss_Sel_Dia/dia_vessel.htm";
	}
	else if(selName=="client")
	{
		wb="/Wss/Wss_Sel_Dia/dia_cli.htm";
		objEva[10]="0";
	}
	else if(selName=="supplier")
	{
		wb="/Wss/Wss_Sel_Dia/dia_cli.htm";
		objEva[10]="1";
	}
	else if(selName=="unit")
	{
		wb="/Wss/Wss_Sel_Dia/dia_unit.htm";
	}
	else if(selName=="clevel")
	{
		wb="/Wss/Wss_Sel_Dia/dia_cle.htm";
	}
	else if(selName=="industry")
	{
		wb="/Wss/Wss_Sel_Dia/dia_ind.htm";
	}
	else if(selName=="csource")
	{
		wb="/Wss/Wss_Sel_Dia/dia_cso.htm";
	}
	else if(selName=="pmode")
	{
		wb="/Wss/Wss_Sel_Dia/dia_pmo.htm";
	}
	else if(selName=="otreason")
	{
		wb="/Wss/Wss_Sel_Dia/dia_otr.htm";
	}
	else if(selName=="ttype")
	{
		wb="/Wss/Wss_Sel_Dia/dia_tty.htm";
	}
	else if(selName=="tmode")
	{
		wb="/Wss/Wss_Sel_Dia/dia_trm.htm";
	}
	else if(selName=="cost_item")
	{
		wb="/Wss/Wss_Sel_Dia/dia_cit.htm";
	}
	else if(selName=="server_item")
	{
		wb="/Wss/Wss_Sel_Dia/dia_sit.htm";
	}
	else if(selName=="ope_item")
	{
		wb="/Wss/Wss_Sel_Dia/dia_oit.htm";
	}
	else if(selName=="doc_item")
	{
		wb="/Wss/Wss_Sel_Dia/dia_dit.htm";
	}
	else if(selName=="open_bank")
	{
		wb="/Wss/Wss_Sel_Dia/dia_oab.htm";
	}
	else if(selName=="ope_no")
	{
		wb="/Wss/Wss_Sel_Dia/dia_opn.htm";
	}

	else
		return;
	showModalDialog(wb,objEva,
		 "status:no;center:yes;help:no;minimize:no;maximize:no;border:thick;statusbar:no;dialogWidth:400px;scroll:none;dialogHeight:330px;overflow:none");
}