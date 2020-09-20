using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.IO.Ports;
using System.Net;

// https://github.com/ollelogdahl/ConsoleGameEngine
using ConsoleGameEngine;

namespace com_reader
{
    class Program
    {
        const int buffer_size = 1048576;
        static int current_buffer_point = 0;
        static int last_buffer_point = 0;
        static byte[] buffer = new byte[buffer_size];
        static byte[] message = new byte[buffer_size];
        static byte[] data = new byte[buffer_size];

        static int message_length;
        static int data_length;
        static int pixel_x;
        static int pixel_y;

        static ConsoleEngine Engine;
        static SerialPort port = null;

        static byte[] settings_data;

        static String str = "";

        // https://stackoverflow.com/questions/311165/how-do-you-convert-a-byte-array-to-a-hexadecimal-string-and-vice-versa
        public static string ByteArrayToString(byte[] ba, int length)
        {
            StringBuilder hex = new StringBuilder(length * 8);
            int j = 0;
            foreach (byte b in ba)
            {
                for (int i = 7; i >= 0; i--)
                {
                    if (((b >> i) & 1) == 1)
                    {
                        hex.Append("1");
                    } else
                    {
                        hex.Append("0");

                    }
                }
                
                        
                j++;
                if(j >= length)
                {
                    break;
                }
            }
            return hex.ToString();
        }

        static void WritePortWithOutput(String output)
        {
            port.WriteLine(output);
            System.Threading.Thread.Sleep(250);
            ReadCom(port);
            str = System.Text.Encoding.Default.GetString(buffer, 0, current_buffer_point);
            current_buffer_point = 0;
            Console.WriteLine(str);
        }

        static void SetBaudSpeed(String com)
        {
            try
            {
                // 115200
                port = new SerialPort(com, 115200, Parity.None, 8, StopBits.One);
                port.Open();
            }
            catch (System.InvalidOperationException e)
            {
                Console.Out.WriteLine(e.Message);
                return;
            }

            Console.WriteLine("Setting speed...");

            WritePortWithOutput("");
            WritePortWithOutput("b");
            WritePortWithOutput("10");
            WritePortWithOutput("15");
            WritePortWithOutput(" ");

            port.Close();
            System.Threading.Thread.Sleep(250);
        }

        static bool GoIntoSpyMode(String com)
        {
            // reopen port at faster speed
            try
            {
                // 115200
                port = new SerialPort(com, 250000, Parity.None, 8, StopBits.One);
                port.Open();
            }
            catch (System.InvalidOperationException e)
            {
                Console.Out.WriteLine(e.Message);
                return false;
            }

            Console.WriteLine("Going into Sniffing Mode...");

            // http://dangerousprototypes.com/docs/Bitbang
            bool BBI01 = false;

            for (int j = 0; j < 40; j++)
            {
                port.Write(settings_data, 0, 1); // mode
                System.Threading.Thread.Sleep(250);
                ReadCom(port);
                str = System.Text.Encoding.Default.GetString(buffer, 0, current_buffer_point);
                Console.WriteLine(str);
                if (str == "BBIO1")
                {
                    current_buffer_point = 0;
                    BBI01 = true;
                    break;
                }
                current_buffer_point = 0;
            }

            if (!BBI01)
            {
                Console.WriteLine("Failed to enter bitbang mode.");
                return false;
            }

            port.Write(settings_data, 1, 1); // i2c mode
            System.Threading.Thread.Sleep(250);
            ReadCom(port);
            current_buffer_point = 0;


            port.Write(settings_data, 2, 1); // i2c sniffer
            System.Threading.Thread.Sleep(250);
            ReadCom(port);
            current_buffer_point = 0;
            return true;
        }

        static void Main(string[] args)
        {
            String com = "COM4";
            if (args.Length == 1)
            {
                com = args[0];
            }

            settings_data = new byte[] {
                0x00, // trigger bitbang mode
                0b00000010, // i2c mode
                0b00001111 // i2c sniffer
            };

            pixel_x = 0;
            pixel_y = 0;

            Console.SetWindowSize(128, 64);
            Engine = new ConsoleEngine(128, 64, 8, 8);
            Console.Title = "RetroLCD.com I2CU Pirate View";

            //SetBaudSpeed(com);

            if(!GoIntoSpyMode(com))
            {
                return;
            }

            Console.WriteLine("Reading Data...");

            bool done = false;
            while (!done)
            {
                ReadCom(port);
                ProcessBuffer();
            }
            port.Close();
        }

        static void LogData()
        {
            string path = @"I2CData.txt";
            // This text is added only once to the file.
            if (!File.Exists(path))
            {
                // Create a file to write to.
                using (StreamWriter sw = File.CreateText(path))
                {
                    sw.WriteLine("Start");
                }
            }

            using (StreamWriter sw = File.AppendText(path))
            {
                sw.WriteLine(ByteArrayToString(data, data_length));
            }
        }

        static void ProcessMessage()
        {
            if (buffer[last_buffer_point] == '[')
            {
                message_length = 0;
            }
            message[message_length] = buffer[last_buffer_point];
            message_length++;

            if (buffer[last_buffer_point] == ']')
            {
                if(message_length > 2)
                {
                    data_length = 0;
                    for(int j = 1; j < message_length - 1; j++)
                    {
                        switch(message[j])
                        {
                            case 0x5c: // \
                                j++;
                                data[data_length] = message[j];
                                data_length++;
                                break;
                        }
                    }
                    if(data_length > 0)
                    {
                        //LogData();

                        //Engine.ClearBuffer();
                        if (data_length == 3)
                        {
                            if (data[0] == 0x78 && data[1] == 0x00 && data[2] == 0x7f)
                            {
                                Engine.DisplayBuffer();
                                pixel_x = 0;
                                pixel_y = 0;
                            }
                        }
                        else
                        {
                            if (data_length == 33)
                            {
                                for (int b = 2; b < 33; b++)
                                {
                                    byte c = data[b];
                                    for(int i = 7; i >= 0; i--)
                                    {
                                        if(((c >> i) & 1) == 1)
                                        {
                                            Engine.SetPixel(new Point(pixel_x, pixel_y + i), (int)ConsoleColor.White, (int)ConsoleColor.White);
                                        } else
                                        {
                                            Engine.SetPixel(new Point(pixel_x, pixel_y + i), (int)ConsoleColor.Black, (int)ConsoleColor.Black);
                                        }
                                    }
                                    pixel_x ++;
                                    if (pixel_x >= 128)
                                    {
                                        pixel_x = 0;
                                        pixel_y+=8;
                                    }
                                    if (pixel_y > 63)
                                    {
                                        pixel_y = 63;
                                    }
                                }
                            }
                        }

                        //Console.WriteLine(ByteArrayToString(data, data_length));
                    }
                    
                }
                message_length = 0;

            }

        }
        static void ProcessBuffer()
        {
            if(last_buffer_point < current_buffer_point)
            {
                while(last_buffer_point < current_buffer_point)
                {
                    ProcessMessage();
                    last_buffer_point++;
                    last_buffer_point %= buffer_size;
                }
            } else
            {
                while (last_buffer_point > current_buffer_point)
                {
                    ProcessMessage();
                    last_buffer_point++;
                    last_buffer_point %= buffer_size;
                    while (last_buffer_point < current_buffer_point)
                    {
                        ProcessMessage();
                        last_buffer_point++;
                        last_buffer_point %= buffer_size;
                    }
                }

            }

        }

        static void ReadCom(SerialPort port)
        {
            while (port.BytesToRead > 0)
            {
                buffer[current_buffer_point] = (byte)port.ReadByte();
                current_buffer_point++;
                current_buffer_point %= buffer_size;
            }
        }
    }
}
