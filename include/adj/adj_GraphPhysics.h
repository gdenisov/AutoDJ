
#pragma once

#include "adj/adj_Adj.h"

#include "cinder/Vector.h"

namespace graph {
    class ParticleSystem;
    class Particle;
}

namespace adj {

typedef std::shared_ptr<graph::Particle> ParticlePtr;
typedef std::shared_ptr<graph::ParticleSystem> ParticleSystemPtr;

class GraphPhysics {
public:
    static GraphPhysics& instance();
    static void cleanup();

    // NOTE: the notion of a parent-child relationship isn't strict in this
    // physics model: specifying a parent simply means that the two node are
    // connected by a spring; there's no sense of directionality (that's set 
    // up in the GraphNode object.
    ParticlePtr create_unconnected_particle();
    ParticlePtr create_particle(); // Attatch the node to a random node
    ParticlePtr create_particle(ParticlePtr parent); // create new attatched to parent

    void update();

    ParticleSystemPtr particle_system() { return p_system_; }

private:
    GraphPhysics();
    void init();

    void setup_new_node(ParticlePtr p, ParticlePtr parent);
    void add_spacers_to_node(ParticlePtr p, ParticlePtr r);
    void make_edge_between(ParticlePtr a, ParticlePtr b);

    // shared_ptrs to all sorts of classes
    ParticleSystemPtr p_system_;

    static GraphPhysics* instance_;

    float spacer_strength_;
    float edge_strength_;
    float edge_length_;
};

}
