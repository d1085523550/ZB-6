using System;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace 在线编程系统
{
    public partial class Form1 : Form
    {
        private ComboBox[] DW_ComBox;
        private Label[] LED0;
        private Label[] LED1;
        private Label[] LED2;
        private TextBox[] ActionName_TextBox;
        private RadioButton[,] Action_RadioButton;
        private ListBox RomList;
        private bool[] ActionButton;
        private Encoding ActionName = Encoding.GetEncoding("gb2312");
        public Form1()
        {
            InitializeComponent();
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            Control.CheckForIllegalCrossThreadCalls = false; //允许跨线程操作
            DW_ComBox = new ComboBox[] { comboBox_DW1_0, comboBox_DW1_1, comboBox_DW1_2, comboBox_DW1_3, comboBox_DW1_4, comboBox_DW1_5, comboBox_DW1_6, comboBox_DW1_7, comboBox_DW2_7, comboBox_DW2_6, comboBox_DW2_5, comboBox_DW2_4, comboBox_DW2_3, comboBox_DW2_2, comboBox_DW2_1, comboBox_DW2_0 };
            LED0 = new Label[] { LED00, LED01, LED02, LED03, LED04, LED05, LED06, LED07 };
            LED1 = new Label[] { LED10, LED11, LED12, LED13, LED14, LED15, LED16, LED17 };
            LED2 = new Label[] { LED20, LED21, LED22, LED23, LED24, LED25, LED26, LED27 };
            ActionName_TextBox = new TextBox[] { text00, text01, text02, text03, text04, text05, text06, text07, text27, text26, text25, text24, text23, text22, text21, text20 };
            Action_RadioButton = new RadioButton[,] { { A000, A001, A002 }, { A010, A011, A012 }, { A020, A021, A022 }, { A030, A031, A032 }, { A040, A041, A042 }, { A050, A051, A052 }, { A060, A061, A062 }, { A070, A071, A072 }, { A270, A271, A272 }, { A260, A261, A262 }, { A250, A251, A252 }, { A240, A241, A242 }, { A230, A231, A232 }, { A220, A221, A222 }, { A210, A211, A212 }, { A200, A201, A202 } };
            RomList = listBox1;
            ActionButton = new bool[18];
            tableLayoutPanel9.Enabled = false;
            tableLayoutPanel10.Enabled = false;
            tableLayoutPanel11.Enabled = false;
            tableLayoutPanel12.Enabled = false;
            tableLayoutPanel13.Enabled = false;
            tableLayoutPanel14.Enabled = false;
            tableLayoutPanel15.Enabled = false;
            tableLayoutPanel16.Enabled = false;
            MessageLabel.Location = new Point(480, 153);
            MessageLabel.Size = new Size(270, 160);
            DWRefresh();
        }
        private void Msg(string n)
        {
            MessageLabel.Text = n;
            MessageLabel.Visible = true;
            timer1.Enabled = true;
        }
        private void tabControl1_Selecting(object sender, TabControlCancelEventArgs e)  //判断选项卡
        {
            switch (e.TabPageIndex)
            {
                case 0:
                    RomList = listBox1;
                    break;
                case 1:
                    RomList = listBox2;
                    break;
                case 2:
                    RomList = listBox3;
                    break;
            }
            RomList.SelectedIndex = -1;
        }
        private void DWRefresh()
        {
            string[] DWdata = new string[16];
            string[] DWmotor = new string[] { "00-1", "00-2", "01-1", "01-2", "02-1", "02-2", "03-1", "03-2", "04-1", "04-2", "05-1", "05-2", "06-1", "06-2", "07-1", "07-2" };
            for (int x = 0; x < 16; x++)
            {
                DW_ComBox[x].Items.Clear();
            }
            if (checkBox1.Checked == true)
            {
                DWdata[0] = "27-1";
                DWdata[1] = "27-2";
            }
            if (checkBox2.Checked == true)
            {
                DWdata[2] = "26-1";
                DWdata[3] = "26-2";
            }
            if (checkBox3.Checked == true)
            {
                DWdata[4] = "25-1";
                DWdata[5] = "25-2";
            }
            if (checkBox4.Checked == true)
            {
                DWdata[6] = "24-1";
                DWdata[7] = "24-2";
            }
            if (checkBox5.Checked == true)
            {
                DWdata[8] = "23-1";
                DWdata[9] = "23-2";
            }
            if (checkBox6.Checked == true)
            {
                DWdata[10] = "22-1";
                DWdata[11] = "22-2";
            }
            if (checkBox7.Checked == true)
            {
                DWdata[12] = "21-1";
                DWdata[13] = "21-2";
            }
            if (checkBox8.Checked == true)
            {
                DWdata[14] = "20-1";
                DWdata[15] = "20-2";
            }
            for (int x = 0; x < 16; x++)
            {
                if (DW_ComBox[x].Enabled == true)
                {
                    DW_ComBox[x].Items.Add("关闭");
                    for (int y = 1; y < 9; y++)
                    {
                        DW_ComBox[x].Items.Add(DWmotor[y * 2 - 2]);
                        DW_ComBox[x].Items.Add(DWmotor[y * 2 - 1]);
                    }
                    for (int y = 0; y < 16; y++)
                    {
                        if (DWdata[y] != null)
                        {
                            DW_ComBox[x].Items.Add(DWdata[y]);
                        }
                    }
                }
            }
        }
        private void A000_Click(object sender, EventArgs e)
        {
            ActionButton[0] = !ActionButton[0];
            Action_RadioButton[0, 0].Checked = ActionButton[0];
        }

        private void A010_Click(object sender, EventArgs e)
        {
            ActionButton[1] = !ActionButton[1];
            Action_RadioButton[1, 0].Checked = ActionButton[1];
        }

        private void A020_Click(object sender, EventArgs e)
        {
            ActionButton[2] = !ActionButton[2];
            Action_RadioButton[2, 0].Checked = ActionButton[2];
        }

        private void A030_Click(object sender, EventArgs e)
        {
            ActionButton[3] = !ActionButton[3];
            Action_RadioButton[3, 0].Checked = ActionButton[3];
        }

        private void A040_Click(object sender, EventArgs e)
        {
            ActionButton[4] = !ActionButton[4];
            Action_RadioButton[4, 0].Checked = ActionButton[4];
        }

        private void A050_Click(object sender, EventArgs e)
        {
            ActionButton[5] = !ActionButton[5];
            Action_RadioButton[5, 0].Checked = ActionButton[5];
        }

        private void A060_Click(object sender, EventArgs e)
        {
            ActionButton[6] = !ActionButton[6];
            Action_RadioButton[6, 0].Checked = ActionButton[6];
        }

        private void A070_Click(object sender, EventArgs e)
        {
            ActionButton[7] = !ActionButton[7];
            Action_RadioButton[7, 0].Checked = ActionButton[7];
        }

        private void A270_Click(object sender, EventArgs e)
        {
            ActionButton[8] = !ActionButton[8];
            Action_RadioButton[8, 0].Checked = ActionButton[8];
        }

        private void A260_Click(object sender, EventArgs e)
        {
            ActionButton[9] = !ActionButton[9];
            Action_RadioButton[9, 0].Checked = ActionButton[9];
        }

        private void A250_Click(object sender, EventArgs e)
        {
            ActionButton[10] = !ActionButton[10];
            Action_RadioButton[10, 0].Checked = ActionButton[10];
        }

        private void A240_Click(object sender, EventArgs e)
        {
            ActionButton[11] = !ActionButton[11];
            Action_RadioButton[11, 0].Checked = ActionButton[11];
        }

        private void A230_Click(object sender, EventArgs e)
        {
            ActionButton[12] = !ActionButton[12];
            Action_RadioButton[12, 0].Checked = ActionButton[12];
        }

        private void A220_Click(object sender, EventArgs e)
        {
            ActionButton[13] = !ActionButton[13];
            Action_RadioButton[13, 0].Checked = ActionButton[13];
        }

        private void A210_Click(object sender, EventArgs e)
        {
            ActionButton[14] = !ActionButton[14];
            Action_RadioButton[14, 0].Checked = ActionButton[14];
        }

        private void A200_Click(object sender, EventArgs e)
        {
            ActionButton[15] = !ActionButton[15];
            Action_RadioButton[15, 0].Checked = ActionButton[15];
        }
        private void button_ComPort_Click(object sender, EventArgs e)
        {
            String[] ComPort;
            comboBox_ComPort.Items.Clear();        //清除数据
            ComPort = System.IO.Ports.SerialPort.GetPortNames();    //扫描可用端口
            for (int i = 0; i < ComPort.Length; i++)
            {
                comboBox_ComPort.Items.Add(ComPort[i]);
            }
            comboBox_ComPort.SelectedIndex = 0;
            if (ComPort.Length > 0)
            {
                button_ComOpen.Enabled = true;
            }
            else
            {
                button_ComOpen.Enabled = false;
            }
        }

        private void button_ComOpen_Click(object sender, EventArgs e)
        {
            if (button_ComOpen.Text == "打开端口")
            {
                serialPort1.PortName = comboBox_ComPort.SelectedItem.ToString();
                try
                {
                    serialPort1.Open();
                    label_LED.BackColor = Color.LimeGreen;
                    button_ComOpen.Text = "关闭端口";
                }
                catch
                {
                    Msg("端口被已被占用！");
                    serialPort1.Close();
                }
            }
            else
            {
                button_ComOpen.Text = "打开端口";
                label_LED.BackColor = Color.Red;
                serialPort1.Close();
            }
        }

        //串口接收数据事件
        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            byte[] ReadData = new byte[serialPort1.BytesToRead];
            int[] DATAS = new int[0];
            bool DataFlag = false;
            int DataNum = 0, SUM = 0;
            serialPort1.Read(ReadData, 0, ReadData.Length);
            for (int i = 0; i < ReadData.Length; i++)
            {
                if (ReadData[i] == 253 && DataFlag == false)
                {
                    DataFlag = true;
                    DATAS = new int[ReadData[i + 1] + 2];
                }
                if (DataFlag)
                {
                    DATAS[DataNum] = ReadData[i];
                    DataNum++;
                    if (DataNum >= DATAS[1] + 2 && DATAS[1] > 0)
                    {
                        DataFlag = false;
                        DataNum = 0;
                        SUM = 0;
                        for (int a = 0; a < DATAS.Length - 1; a++)
                            SUM += DATAS[a];
                        if (SUM % 256 == DATAS[DATAS.Length - 1])
                        {
                            label_Read_Data.Text = "";
                            for (int a = 0; a < DATAS.Length; a++)
                            {
                                label_Read_Data.Text += DATAS[a].ToString("X") + " ";
                            }
                            Data_FL(DATAS);
                        }

                    }
                }
            }
            label_Write_Data.Text = "";
            label_Write_Data.Text = ActionName.GetString(ReadData);
        }
        private void Data_FL(int[] Data) //接收数据分类并处理
        {
            if (Data[2] == 0x50)
            {
                Msg("成功！");
            }
            else if (Data[2] <= 0x60)
            {

            }
            else if (Data[2] == 0x61)
            {

            }
            else if (Data[2] == 0x62)
            {

            }
            else if (Data[2] == 0x63)
            {

            }
            else if (Data[2] == 0x64)
            {

            }
            else if (Data[2] == 0x65)
            {

            }
            else if (Data[2] == 0x70)
            {

            }
            else if (Data[2] == 0x71)
            {

            }
            else if (Data[2] == 0x80)
            {

            }
            else if (Data[2] == 0x90)
            {

            }

        }
        //********************************************************************端口状态检测
        private void checkBox9_CheckedChanged(object sender, EventArgs e)
        {
            byte[] i = new byte[] { 0xfd, 0x01, 0x02 };
            if (checkBox9.Checked == true)
            {
                i[2] = 0x01;
                serialPort1.Write(i, 0, i.Length);
            }
            else
            {
                i[2] = 0x00;
                serialPort1.Write(i, 0, i.Length);

            }
        }
        //***********************************************************************扩展口选择 

        private void MotorOrDW(int x)
        {
            CheckBox[] MotorCheckbox = new CheckBox[] { checkBox1, checkBox2, checkBox3, checkBox4, checkBox5, checkBox6, checkBox7, checkBox8 };
            TableLayoutPanel[] Ation = new TableLayoutPanel[] { tableLayoutPanel16, tableLayoutPanel15, tableLayoutPanel14, tableLayoutPanel13, tableLayoutPanel12, tableLayoutPanel11, tableLayoutPanel10, tableLayoutPanel9 };
            ComboBox[] DWXZ = new ComboBox[] { comboBox_DW2_7, comboBox_DW2_6, comboBox_DW2_5, comboBox_DW2_4, comboBox_DW2_3, comboBox_DW2_2, comboBox_DW2_1, comboBox_DW2_0 };
            if (x == 0)
            {
                for (int n = 0; n < 8; n++)
                {
                    if (MotorCheckbox[x].Checked == true)
                    {
                        Ation[x].Enabled = true;
                        DWXZ[x].Enabled = false;
                    }
                    else
                    {
                        Ation[x].Enabled = false;
                        DWXZ[x].Enabled = true;
                    }
                }
                DWRefresh();
            }
            else
            {
                if (MotorCheckbox[x - 1].Checked == true)
                {
                    Ation[x - 1].Enabled = true;
                    DWXZ[x - 1].Enabled = false;
                }
                else
                {
                    Ation[x - 1].Enabled = false;
                    DWXZ[x - 1].Enabled = true;
                }
                RefreshTimer.Enabled = false;
                RefreshTimer.Enabled = true;
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            MotorOrDW(1);
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            MotorOrDW(2);
        }

        private void checkBox3_CheckedChanged(object sender, EventArgs e)
        {
            MotorOrDW(3);
        }

        private void checkBox4_CheckedChanged(object sender, EventArgs e)
        {
            MotorOrDW(4);
        }

        private void checkBox5_CheckedChanged(object sender, EventArgs e)
        {
            MotorOrDW(5);
        }

        private void checkBox6_CheckedChanged(object sender, EventArgs e)
        {
            MotorOrDW(6);
        }

        private void checkBox7_CheckedChanged(object sender, EventArgs e)
        {
            MotorOrDW(7);
        }

        private void checkBox8_CheckedChanged(object sender, EventArgs e)
        {
            MotorOrDW(8);
        }
        //***************************************************************************************程序段编写常用按钮
        private int YZ()
        {
            if (textBox17.Text != "" && textBox18.Text != "")
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        private void AddList()
        {
            string IOPut;
            int Sound, Time;
            IOPut = "No:";
            if (RomList.Items.Count < 100)
            {
                IOPut += "0";
            }
            if (RomList.Items.Count < 10)
            {
                IOPut += "0";
            }
            IOPut += RomList.Items.Count.ToString() + "|";
            for (int x = 0; x < 16; x++)
            {
                if (Action_RadioButton[x, 0].Checked == true)
                {
                    IOPut += "1";
                }
                else if (Action_RadioButton[x, 1].Checked == true)
                {
                    IOPut += "2";
                }
                else if (Action_RadioButton[x, 2].Checked == true)
                {
                    IOPut += "3";
                }
                else
                {
                    IOPut += "0";
                }
                if (x < 15)
                {
                    IOPut += ",";
                }
            }
            IOPut += "|Sound:";
            Sound = int.Parse(textBox18.Text);
            if (Sound < 100)
            {
                IOPut += "0";
            }
            if (Sound < 10)
            {
                IOPut += "0";
            }
            IOPut += textBox18.Text + "|Time:";
            Time = int.Parse(textBox17.Text);
            if (Time < 10000)
            {
                IOPut += "0";
            }
            if (Time < 1000)
            {
                IOPut += "0";
            }
            if (Time < 100)
            {
                IOPut += "0";
            }
            if (Time < 10)
            {
                IOPut += "0";
            }
            IOPut += textBox17.Text;
            RomList.Items.Add(IOPut);
        }
        private void ToListTest(int Setp)
        {
            int y;
            y = RomList.Items.Count - Setp;
            for (int x = 0; x < y; x++)
            {
                listTest.Items.Add(RomList.Items[Setp]);
                RomList.Items.RemoveAt(Setp);
            }
        }
        private void ReturnListTest(int Setp)
        {
            string i;
            for (int x = 0; x < listTest.Items.Count; x++)
            {
                i = listTest.Items[x].ToString();
                i = i.Remove(0, 6);
                i = (Setp + x).ToString() + i;
                if ((Setp + x) < 10)
                {
                    i = "0" + i;
                }
                if ((Setp + x) < 100)
                {
                    i = "0" + i;
                }
                i = "No:" + i;
                RomList.Items.Add(i);
            }
        }
        private void button1_Click(object sender, EventArgs e)  //添加
        {
            if (YZ() == 0)
            {
                return;
            }
            AddList();
            RomList.SelectedIndex = RomList.Items.Count - 1;
            RomList.SelectedIndex = -1;
        }

        private void button2_Click(object sender, EventArgs e)  //插入
        {
            int Setp;
            if (YZ() == 0)
            {
                return;
            }
            Setp = RomList.SelectedIndex;
            if (Setp < 0)
            {
                return;
            }
            ToListTest(Setp);
            AddList();
            ReturnListTest(Setp + 1);
            listTest.Items.Clear();
            RomList.SelectedIndex = Setp;
        }

        private void button3_Click(object sender, EventArgs e)  //修改
        {
            int Setp;
            if (YZ() == 0)
            {
                return;
            }
            Setp = RomList.SelectedIndex;
            if (Setp < 0)
            {
                return;
            }
            ToListTest(Setp);
            AddList();
            listTest.Items.RemoveAt(0);
            ReturnListTest(Setp + 1);
            listTest.Items.Clear();
            RomList.SelectedIndex = Setp;
        }

        private void button13_Click(object sender, EventArgs e)  //删除
        {
            int Setp, y;
            if (YZ() == 0)
            {
                return;
            }
            y = RomList.SelectedItems.Count;
            Setp = RomList.SelectedIndex;
            if (Setp < 0)
            {
                return;
            }
            ToListTest(Setp);
            for (int i = 0; i < y; i++)
            {
                listTest.Items.RemoveAt(0);
            }
            ReturnListTest(Setp);
            listTest.Items.Clear();
            if (RomList.Items.Count != 0)
            {
                RomList.SelectedIndex = Setp;
                RomList.SelectedIndex = -1;
            }
        }

        private void button4_Click(object sender, EventArgs e)   //清除
        {
            RomList.Items.Clear();
        }
        private void 全选ToolStripMenuItem_Click(object sender, EventArgs e)  //全选
        {
            for (int x = 0; x < RomList.Items.Count; x++)
            {
                RomList.SelectedIndex = x;
            }

        }

        private void 复制ToolStripMenuItem_Click(object sender, EventArgs e)  //复制
        {
            int Setp;
            string[] i = new string[RomList.SelectedItems.Count];
            if (YZ() == 0)
            {
                return;
            }
            Setp = RomList.SelectedItems.Count;
            if (Setp < 0)
            {
                return;
            }
            listCopy.Items.Clear();
            RomList.SelectedItems.CopyTo(i, 0);
            for (int x = 0; x < i.Length; x++)
            {
                listCopy.Items.Add(i[x]);
            }
        }

        private void 粘贴ToolStripMenuItem_Click(object sender, EventArgs e)  //粘贴
        {
            int Setp, y;
            string[] i = new String[listCopy.Items.Count];
            string s;
            if (YZ() == 0)
            {
                return;
            }
            listCopy.Items.CopyTo(i, 0);
            Setp = RomList.SelectedIndex + 1;
            y = RomList.Items.Count - Setp;
            for (int x = 0; x < y; x++)
            {
                listCopy.Items.Add(RomList.Items[Setp]);
                RomList.Items.RemoveAt(Setp);
            }
            for (int x = 0; x < listCopy.Items.Count; x++)
            {
                s = listCopy.Items[x].ToString();
                s = s.Remove(0, 6);
                s = (Setp + x).ToString() + s;
                if ((Setp + x) < 10)
                {
                    s = "0" + s;
                }
                if ((Setp + x) < 100)
                {
                    s = "0" + s;
                }
                s = "No:" + s;
                RomList.Items.Add(s);
            }
            listCopy.Items.Clear();
            for (int x = 0; x < i.Length; x++)
            {
                listCopy.Items.Add(i[x]);
            }
        }

        private void 删除ToolStripMenuItem_Click(object sender, EventArgs e)  //删除
        {
            button13_Click(0, EventArgs.Empty);
        }
        //******************************************************************************************载入与保存    
        private void button9_Click(object sender, EventArgs e)   //下载
        {
            byte[] name = ActionName.GetBytes(text00.Text);
            serialPort1.Write(name, 0, name.Length);
        }
        private void button10_Click(object sender, EventArgs e)  //上载
        {

        }
        private void button11_Click(object sender, EventArgs e)  //保存
        {
            StreamWriter BCWJ;
            CheckBox[] MotorCheckbox = new CheckBox[] { checkBox1, checkBox2, checkBox3, checkBox4, checkBox5, checkBox6, checkBox7, checkBox8 };
            string RomData = "";
            string Name = "";
            string Motor = "";
            string DW = "";
            string ROM0 = "";
            string ROM1 = "";
            string ROM2 = "";
            for (int x = 0; x < 16; x++)
            {
                Name += ActionName_TextBox[x].Text + ",";
            }
            if (Name != "")
                Name = Name.Remove(Name.Length - 1, 1);

            for (int x = 0; x < 8; x++)
            {
                if (MotorCheckbox[x].Checked == true)
                {
                    Motor += "1,";
                }
                else
                {
                    Motor += "0,";
                }
            }
            if (Motor != "")
                Motor = Motor.Remove(Motor.Length - 1, 1);

            for (int x = 0; x < 16; x++)
            {
                DW += DW_ComBox[x].SelectedItem + ",";
            }
            if (DW != "")
                DW = DW.Remove(DW.Length - 1, 1);

            for (int x = 0; x < listBox1.Items.Count; x++)
            {
                ROM0 += listBox1.Items[x] + "@";
            }
            if (ROM0 != "")
                ROM0 = ROM0.Remove(ROM0.Length - 1, 1);

            for (int x = 0; x < listBox2.Items.Count; x++)
            {
                ROM1 += listBox2.Items[x] + "@";
            }
            if (ROM1 != "")
                ROM1 = ROM1.Remove(ROM1.Length - 1, 1);

            for (int x = 0; x < listBox3.Items.Count; x++)
            {
                ROM2 += listBox3.Items[x] + "@";
            }
            if (ROM2 != "")
                ROM2 = ROM2.Remove(ROM2.Length - 1, 1);

            RomData = Name + "^" + Motor + "^" + DW + "^" + ROM0 + "^" + ROM1 + "^" + ROM2;
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)  //最终数据保存
            {
                BCWJ = new StreamWriter(saveFileDialog1.FileName);
                BCWJ.Write(RomData);
                BCWJ.Close();
            }
            else
            {
                Msg("保存失败！");
            }
        }
        private void button12_Click(object sender, EventArgs e)  //载入
        {
            StreamReader ZRWJ;
            string Data;
            CheckBox[] MotorCheckbox = new CheckBox[] { checkBox1, checkBox2, checkBox3, checkBox4, checkBox5, checkBox6, checkBox7, checkBox8 };
            string Name = "";
            string Motor = "";
            string DW = "";
            string ROM0 = "";
            string ROM1 = "";
            string ROM2 = "";
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                ZRWJ = new StreamReader(openFileDialog1.FileName);
                Data = ZRWJ.ReadToEnd();
                ZRWJ.Close();

            }
            else
            {
                Msg("载入失败！");
                return;
            }
            Name = Data.Remove(Data.IndexOf("^"));
            Data = Data.Remove(0, Data.IndexOf("^") + 1);

            Motor = Data.Remove(Data.IndexOf("^"));
            Data = Data.Remove(0, Data.IndexOf("^") + 1);

            DW = Data.Remove(Data.IndexOf("^"));
            Data = Data.Remove(0, Data.IndexOf("^") + 1);

            ROM0 = Data.Remove(Data.IndexOf("^"));
            Data = Data.Remove(0, Data.IndexOf("^") + 1);

            ROM1 = Data.Remove(Data.IndexOf("^"));
            Data = Data.Remove(0, Data.IndexOf("^") + 1);

            ROM2 = Data;

            for (int x = 0; x < 15; x++)
            {
                ActionName_TextBox[x].Text = Name.Remove(Name.IndexOf(","));
                Name = Name.Remove(0, Name.IndexOf(",") + 1);
            }
            ActionName_TextBox[15].Text = Name;

            for (int x = 0; x < 7; x++)
            {
                if (Motor.Remove(1) == "1")
                {
                    MotorCheckbox[x].Checked = true;
                }
                else
                {
                    MotorCheckbox[x].Checked = false;
                }
                Motor = Motor.Remove(0, 2);
            }
            if (Motor == "1")
            {
                MotorCheckbox[7].Checked = true;
            }
            else
            {
                MotorCheckbox[7].Checked = false;
            }
            MotorOrDW(0);

            for (int x = 0; x < 15; x++)
            {
                if (DW.Remove(DW.IndexOf(",")) != null)
                    DW_ComBox[x].SelectedIndex = DW_ComBox[x].Items.IndexOf(DW.Remove(DW.IndexOf(",")));
                DW = DW.Remove(0, DW.IndexOf(",") + 1);
            }
            DW_ComBox[15].SelectedIndex = DW_ComBox[15].Items.IndexOf(DW);

            listBox1.Items.Clear();
            while (ROM0.IndexOf("@") != -1)
            {
                listBox1.Items.Add(ROM0.Remove(ROM0.IndexOf("@")));
                ROM0 = ROM0.Remove(0, ROM0.IndexOf("@") + 1);
            }
            if (ROM0 != "")
                listBox1.Items.Add(ROM0);

            listBox2.Items.Clear();
            while (ROM1.IndexOf("@") != -1)
            {
                listBox2.Items.Add(ROM1.Remove(ROM1.IndexOf("@")));
                ROM1 = ROM1.Remove(0, ROM1.IndexOf("@") + 1);
            }
            if (ROM1 != "")
                listBox2.Items.Add(ROM1);

            listBox3.Items.Clear();
            while (ROM2.IndexOf("@") != -1)
            {
                listBox3.Items.Add(ROM2.Remove(ROM2.IndexOf("@")));
                ROM2 = ROM2.Remove(0, ROM2.IndexOf("@") + 1);
            }
            if (ROM2 != "")
                listBox3.Items.Add(ROM2);

        }
        //**************************************************************************正则表达式
        private void textBox17_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == 0x20) e.KeyChar = (char)0; //禁止空格
            if (e.KeyChar != '\b')
            {
                if (e.KeyChar < '0' || e.KeyChar > '9')
                {
                    e.KeyChar = (char)0;
                }
            }
        }
        private void textBox17_TextChanged(object sender, EventArgs e)
        {
            int i;
            if (textBox17.Text == "")
            {
                textBox17.Text = "0";
            }
            else
            {
                i = int.Parse(textBox17.Text);
                if (i > 65535)
                {
                    Msg("单段最大时间不能超过65535！");
                    i = 65535;
                }
                textBox17.Text = i.ToString();
            }
        }
        private void textBox18_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == 0x20) e.KeyChar = (char)0; //禁止空格
            if (e.KeyChar != '\b')
            {
                if (e.KeyChar < '0' || e.KeyChar > '9')
                {
                    e.KeyChar = (char)0;
                }
            }
        }

        private void textBox18_TextChanged(object sender, EventArgs e)
        {
            int i;
            if (textBox18.Text == "")
            {
                textBox18.Text = "0";
            }
            else
            {
                i = int.Parse(textBox18.Text);
                if (i > 255)
                {
                    Msg("声音序号不能大于255！");
                    i = 0;
                    textBox18.Text = i.ToString();
                }
            }
        }

        private void timer1_Tick(object sender, EventArgs e)  //Message定时器
        {
            MessageLabel.Visible = false;
            timer1.Enabled = false;
            timer1.Interval = 1200;
        }

        private void RefreshTimer_Tick(object sender, EventArgs e) //延时刷新
        {
            RefreshTimer.Enabled = false;
            DWRefresh();
        }

        private void listBox1_MouseDown(object sender, MouseEventArgs e)
        {
            string[] data;
            string[] Motor;
            string[] Sound;
            string[] Time;
            if (listBox1.SelectedIndex == -1)
            {
                return;
            }
            data = listBox1.SelectedItem.ToString().Split('|');
            Motor = data[1].Split(',');
            Sound = data[2].Split(':');
            Time = data[3].Split(':');
            for (int x = 0; x < Motor.Length; x++)
            {
                if (Motor[x] == "0")
                {
                    Action_RadioButton[x, 0].Checked = false;
                    Action_RadioButton[x, 1].Checked = false;
                    Action_RadioButton[x, 2].Checked = false;
                }
                else if (Motor[x] == "1")
                {
                    Action_RadioButton[x, 0].Checked = true;
                    Action_RadioButton[x, 1].Checked = false;
                    Action_RadioButton[x, 2].Checked = false;

                }
                else if (Motor[x] == "2")
                {
                    Action_RadioButton[x, 0].Checked = false;
                    Action_RadioButton[x, 1].Checked = true;
                    Action_RadioButton[x, 2].Checked = false;
                }
                else if (Motor[x] == "3")
                {
                    Action_RadioButton[x, 0].Checked = false;
                    Action_RadioButton[x, 1].Checked = false;
                    Action_RadioButton[x, 2].Checked = true;
                }

            }
            textBox18.Text = int.Parse(Sound[1]).ToString();
            textBox17.Text = int.Parse(Time[1]).ToString();
        }

        private void listBox2_MouseDown(object sender, MouseEventArgs e)
        {
            string[] data;
            string[] Motor;
            string[] Sound;
            string[] Time;
            if (listBox2.SelectedIndex == -1)
            {
                return;
            }
            data = listBox2.SelectedItem.ToString().Split('|');
            Motor = data[1].Split(',');
            Sound = data[2].Split(':');
            Time = data[3].Split(':');
            for (int x = 0; x < Motor.Length; x++)
            {
                if (Motor[x] == "0")
                {
                    Action_RadioButton[x, 0].Checked = false;
                    Action_RadioButton[x, 1].Checked = false;
                    Action_RadioButton[x, 2].Checked = false;
                }
                else if (Motor[x] == "1")
                {
                    Action_RadioButton[x, 0].Checked = true;
                    Action_RadioButton[x, 1].Checked = false;
                    Action_RadioButton[x, 2].Checked = false;

                }
                else if (Motor[x] == "2")
                {
                    Action_RadioButton[x, 0].Checked = false;
                    Action_RadioButton[x, 1].Checked = true;
                    Action_RadioButton[x, 2].Checked = false;
                }
                else if (Motor[x] == "3")
                {
                    Action_RadioButton[x, 0].Checked = false;
                    Action_RadioButton[x, 1].Checked = false;
                    Action_RadioButton[x, 2].Checked = true;
                }

            }
            textBox18.Text = int.Parse(Sound[1]).ToString();
            textBox17.Text = int.Parse(Time[1]).ToString();
        }

        private void listBox3_MouseDown(object sender, MouseEventArgs e)
        {
            string[] data;
            string[] Motor;
            string[] Sound;
            string[] Time;
            if (listBox3.SelectedIndex == -1)
            {
                return;
            }
            data = listBox3.SelectedItem.ToString().Split('|');
            Motor = data[1].Split(',');
            Sound = data[2].Split(':');
            Time = data[3].Split(':');
            for (int x = 0; x < Motor.Length; x++)
            {
                if (Motor[x] == "0")
                {
                    Action_RadioButton[x, 0].Checked = false;
                    Action_RadioButton[x, 1].Checked = false;
                    Action_RadioButton[x, 2].Checked = false;
                }
                else if (Motor[x] == "1")
                {
                    Action_RadioButton[x, 0].Checked = true;
                    Action_RadioButton[x, 1].Checked = false;
                    Action_RadioButton[x, 2].Checked = false;

                }
                else if (Motor[x] == "2")
                {
                    Action_RadioButton[x, 0].Checked = false;
                    Action_RadioButton[x, 1].Checked = true;
                    Action_RadioButton[x, 2].Checked = false;
                }
                else if (Motor[x] == "3")
                {
                    Action_RadioButton[x, 0].Checked = false;
                    Action_RadioButton[x, 1].Checked = false;
                    Action_RadioButton[x, 2].Checked = true;
                }

            }
            textBox18.Text = int.Parse(Sound[1]).ToString();
            textBox17.Text = int.Parse(Time[1]).ToString();
        }
    }
}
