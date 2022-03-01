using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.IO.Ports;

namespace 串口控制
{
    public partial class Form1 : Form
    {
        //device 1
        const byte DeviceOpen1 = 0x01;
        const byte DeviceClose1 = 0x81;
        //device 2
        const byte DeviceOpen2 = 0x02;
        const byte DeviceClose2 = 0x82;
        //device 3
        const byte DeviceOpen3 = 0x03;
        const byte DeviceClose3 = 0x83;
        //SerialPort Write Buffer
        bool Button1Statue;
        byte[] SerialPortDataBuffer = new byte[1];
        public Form1()
        {
            InitializeComponent();                                      //窗口构造
        }
        private void button1_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)                                     //串口打开就关闭
            {
                try
                {
                    serialPort1.Close();
                }
                catch { }                                               //确保万无一失
                //button1.Text = "打开串口";
                button1.BackgroundImage = Properties.Resources.Image2;  //灭
                Button1Statue = false;                                  //按钮状态
            }
            else
            {
                try
                {
                    serialPort1.PortName = comboBox1.Text;              //端口号
                    serialPort1.Open();                                 //打开端口
                    //button1.Text = "关闭串口";
                    button1.BackgroundImage = Properties.Resources.Image1;//亮
                    Button1Statue = true;                                //按钮状态
                }
                catch
                {
                    MessageBox.Show("串口打开失败","错误");
                }
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            SearchAndAddSerialToComboBox(serialPort1, comboBox1);
        }

        private void WriteByteToSerialPort(byte data)                   //单字节写入串口
        {
            byte[] Buffer = new byte [2]{0x00, data };                       //定义数组
            if (serialPort1.IsOpen)                                     //传输数据的前提是端口已打开
            {
                try
                {
                    serialPort1.Write(Buffer, 0, 2);                    //写数据
                }
                catch 
                {
                    MessageBox.Show("串口数据发送出错，请检查.","错误");//错误处理
                }
            }
        }

        private void SearchAndAddSerialToComboBox(SerialPort MyPort,ComboBox MyBox)
        {                                                               //将可用端口号添加到ComboBox
            string[] MyString = new string[20];                         //最多容纳20个，太多会影响调试效率
            string Buffer;                                              //缓存
            MyBox.Items.Clear();                                        //清空ComboBox内容
            for (int i = 1; i < 20; i++)                                //循环
            {
                try                                                     //核心原理是依靠try和catch完成遍历
                {
                    Buffer = "COM" + i.ToString();
                    MyPort.PortName = Buffer;
                    MyPort.Open();                                      //如果失败，后面的代码不会执行
                    MyString[i - 1] = Buffer;
                    MyBox.Items.Add(Buffer);                            //打开成功，添加至下俩列表
                    MyPort.Close();                                     //关闭
                }
                catch 
                {
                	
                }
            }
            MyBox.Text = MyString[0];                                   //初始化
        }

        private void button2_Click(object sender, EventArgs e)
        {
            int i = 0;
            try
            {
                i = Convert.ToInt32(textBox1.Text.Substring(0, 2));    //先处理两位数，如果出错就处理一位数
            }
            catch
            {
                try
                {
                    i = Convert.ToInt32(textBox1.Text.Substring(0, 1));//处理一位数
                }
                catch 
                {
                    MessageBox.Show("请输入正确的数字");              //错误提示
                    return;                                           //退出函数
                }
            }
            if (serialPort1.IsOpen)                                  //避免定时器浪费时间和用户等待
            {
                if (i == 0)                                          //如果是0的话程序认为是定时模式关
                {
                    //MessageBox.Show("请输入大于0的数字","提示");//WriteByteToSerialPort(DeviceOpen1);
                    return;
                }
                else
                {
                    timer1.Interval = i * 1000;                     //可以这样写，不需要计数器
                    timer1.Start();                                 //开定时器
                    button2.Enabled = false;                        //开按钮不能按了…
                }
            }
        }

        private void button1_MouseHover(object sender, EventArgs e)
        {
            button1.BackgroundImage = Properties.Resources.Image3;//鼠标指上去则使用Image3
        }

        private void button1_MouseLeave(object sender, EventArgs e)
        {
            if (Button1Statue)                                    //鼠标移开，返回原来状态
            {
                button1.BackgroundImage = Properties.Resources.Image1;
            }
            else
            {
                button1.BackgroundImage = Properties.Resources.Image2;
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            try
            {
                timer1.Stop();                                   //如果定时器没开，则错误处理
            }
            catch
            {
            	
            }
            button2.Enabled = true;
            WriteByteToSerialPort(DeviceClose1);                         //器件一关
        }

        private void button5_Click(object sender, EventArgs e)
        {
            WriteByteToSerialPort(DeviceOpen2);                         //器件二开
        }

        private void button4_Click(object sender, EventArgs e)
        {
            WriteByteToSerialPort(DeviceClose2);                        //器件二关
        }

        private void button7_Click(object sender, EventArgs e)
        {
            WriteByteToSerialPort(DeviceOpen3);                         //器件三开
        }

        private void button6_Click(object sender, EventArgs e)
        {
            WriteByteToSerialPort(DeviceClose3);                        //器件三关
        }

        private void button8_Click(object sender, EventArgs e)
        {
            SearchAndAddSerialToComboBox(serialPort1, comboBox1);       //扫描并讲课用串口添加至下拉列表
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            button2.Enabled = true;                                     //开按钮可以按
            timer1.Stop();                                              //一定要先关闭定时器
            //MessageBox.Show(null);
            WriteByteToSerialPort(DeviceClose1);                         //器件一关
        }
    }
}