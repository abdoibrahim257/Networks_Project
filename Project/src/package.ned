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
        node.out --> node1.in[1];
        node1.out --> node.in[1];
        coordinator.out[0] --> node.in[0];
        coordinator.out[1] --> node1.in[0];
}

