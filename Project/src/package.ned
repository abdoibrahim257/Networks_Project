package project;

@license(LGPL);
//
// TODO documentation
//
network Network
{
    @display("bgb=488,321");
    submodules:
        node: Node {
            @display("p=99,111;i=abstract/penguin");
        }
        node1: Node {
            @display("p=307,111;i=abstract/penguin,cyan");
        }
        coordinator: Coordinator {
            @display("p=205,193;i=block/cogwheel");
        }
    connections:
        node.out[0] --> node1.in[0];
        node1.out[0] --> node.in[0];
        node.out[0] --> coordinator.in[0];
        coordinator.out[0] --> node.in[0];
        coordinator.out[0] --> node1.in[0];
        node1.out[0] --> coordinator.in[0];
}

