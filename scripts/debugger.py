"""
export IRIS_HOME="/opt/FVP_Corstone_1000/Iris"
export PYTHONPATH=$IRIS_HOME/Python:$PYTHONPATH
"""

import sys 
#os.environ['IRIS_HOME'] = '/opt/FVP_Corstone_1000/Iris'
#os.environ['PYTHONPATH'] = os.environ['IRIS_HOME'] + "/Python" #+ ":" + os.environ['PYTHONPATH']

sys.path.insert(0, '/opt/FVP_Corstone_1000/Iris/Python')

from iris import debug

import cmd, sys

class IrisDebugger(cmd.Cmd):
    intro ="Bem vindo ao debugger que fiz porque sou pobre"
    prompt = 'iris> '
    
    model = None
    cpu = None

    def do_target(self, arg):
        args = arg.split()
        if self.model is None:
            if args[0] == "remote":
                connect = ['127.0.0.1', 7100]
                if len(args) > 1:
                    connect = args.split(':')
                    connect[1] = int(connect[1])
                
                self.model = debug.NetworkModel(*connect)
                print(f"Connected to {connect[0]} on port {connect[1]}")
                self.cpu = self.model.get_cpus()[0]
        else:
            print("Already connected to a model. Disconnect it first")

    def do_b(self, arg):
        if self.model is not None:
            address = int(arg, 16)
            self.cpu.add_bpt_mem(address)

    def do_info_register(self, arg):
        if self.model is not None:
            print(self.cpu.get_register_info())
        
    

    def do_EOF(self, arg):
        return True

if __name__ == '__main__':
    IrisDebugger().cmdloop()

#model = debug.NetworkModel("localhost",7100)
#cpu = model.get_cpus()[0]
#cpu.add_bpt_mem(0x38000000)

#model.run()