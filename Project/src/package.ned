package project;

@license(LGPL);
//
// TODO documentation
//

network Network
{
    @display("bgb=488,321");
    submodules:
        node0: Node {
            @display("p=99,111;i=abstract/penguin");
        }
        node1: Node {
            @display("p=307,111;i=abstract/penguin,cyan");
        }
        coordinator: Coordinator {
            @display("p=205,193;i=block/cogwheel");
        }
    connections:
        node0.out --> node1.in[1];
        node1.out --> node0.in[1];
        coordinator.out[0] --> node0.in[0];
        coordinator.out[1] --> node1.in[0];
}

