using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Singleton
{
    public sealed class Singleton1
    {
        private Singleton1()
        {

        }

        private static Singleton1 instance = null;
        public static Singleton1 Instance
        {
            get
            {
                if (instance == null)
                    instance = new Singleton1();
                return instance;
            }
        }
    }

    public sealed class Singlenton2
    {
        private Singlenton2() { }

        private static readonly object syncobj = new object();

        private static Singlenton2 instance = null;
        public static Singlenton2 Instance
        {
            get
            {
                lock (syncobj)
                {
                    if (instance == null)
                        instance = new Singlenton2();
                }

                return instance;
            }
        }
    }

    public sealed class Singleton3
    {
        private Singleton3()
        {

        }

        private static object syncobj = new object();

        private static Singleton3 instance = null;
        public static Singleton3 Instance
        {
            get
            {
                if (instance == null)
                {
                    lock (syncobj)
                    {
                        if (instance == null)
                            instance = new Singleton3();
                    }
                }

                return instance;
            }
        }

    }

    public sealed class Singleton4
    {
        private Singleton4()
        {

        }

        public static void Print()
        {
            Console.WriteLine("Singleton4 Print");
        }

        private static Singleton4 instance = new Singleton4();
        public static Singleton4 Instance
        {
            get
            {
                return instance;
            }
        }
    }

    public sealed class Singleton5
    {
        Singleton5()
        {

        }

        public static void Print()
        {
            Console.WriteLine("Singleotn5 Print");
        }

        public static Singleton5 Instance
        {
            get
            {
                return Nested.instance;
            }
        }

        class Nested 
        {
            static Nested()
            {

            }

            internal static readonly Singleton5 instance = new Singleton5();
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            Singleton4.Print();
            Singleton5.Print();
        }
    }
}
