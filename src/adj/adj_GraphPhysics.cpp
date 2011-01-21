
#include "cinder/Vector.h"
#include "cinder/Rand.h"

#include "graph/graph_ParticleSystem.h"
#include "graph/graph_Particle.h"

#include "adj/adj_GraphPhysics.h"

namespace adj {

GraphPhysics* GraphPhysics::instance_ = NULL;

GraphPhysics::GraphPhysics() {
    spacer_strength_ = 1000.0f;
    edge_length_ = 20.0f;
    edge_strength_ = 0.2f;
}

void GraphPhysics::init() {
    p_system_ = ParticleSystemPtr(new graph::ParticleSystem(ci::Vec2f::zero(), 0.1f));
    p_system_->init();
    p_system_->clear();
}

void GraphPhysics::update() {
    p_system_->tick();
}

ParticlePtr GraphPhysics::create_unconnected_particle() {
    return p_system_->make_particle();
}

ParticlePtr GraphPhysics::create_particle() {
    ci::Rand rand;
    rand.randomize();

    ParticlePtr p = p_system_->make_particle();

    ParticlePtr q;
    do {
        q = p_system_->particles_[rand.randInt(p_system_->particles_.size() - 1)];
    } while (p.get() == q.get());

    setup_new_node(p, q);

    return p;
}

ParticlePtr GraphPhysics::create_particle(ParticlePtr q) {
    ParticlePtr p = p_system_->make_particle();

    setup_new_node(p, q);


    return p;
}

ParticlePtr GraphPhysics::create_box_particle(ParticlePtr parent) {
    ParticlePtr p = p_system_->make_particle();

    // separate the box from all other boxes
    for (std::vector<ParticlePtr>::iterator it = boxes_.begin(); it != boxes_.end(); ++it) {
        make_separation_between(p, *it);
    }

    boxes_.push_back(p);

    make_separation_between(p, parent);
    make_edge_between(p, parent, edge_length_ * 2.0f, edge_strength_);

    ci::Rand rand;
    rand.randomize();

    p->position() = parent->position() + ci::Vec2f(rand.randFloat(-1.0f, 1.0f),
        rand.randFloat(-1.0f, 1.0f));

    return p;
}

void GraphPhysics::setup_new_node(ParticlePtr p, ParticlePtr q) {
    add_spacers_to_node(p, q);
    make_edge_between(p, q);

    ci::Rand rand;
    rand.randomize();

    p->position() = q->position() + ci::Vec2f(rand.randFloat(-1.0f, 1.0f),
        rand.randFloat(-1.0f, 1.0f));
}

void GraphPhysics::add_spacers_to_node(ParticlePtr p, ParticlePtr r) {
    for (std::vector<ParticlePtr>::iterator it = p_system_->particles_.begin();
        it != p_system_->particles_.end(); ++it) {

        ParticlePtr q = *it;

        if (p.get() != q.get() && p.get() != r.get())
            make_separation_between(p, q);
    }
}

void GraphPhysics::make_edge_between(ParticlePtr a, ParticlePtr b) {
    p_system_->make_spring(*(a.get()), *(b.get()), edge_strength_,
        edge_strength_, edge_length_);
}

void GraphPhysics::make_edge_between(ParticlePtr a, ParticlePtr b, float length,
    float strength) {
    p_system_->make_spring(*(a.get()), *(b.get()), strength,
        strength, length);
}

void GraphPhysics::make_separation_between(ParticlePtr a, ParticlePtr b) {
    p_system_->make_attraction(*(a.get()), *(b.get()),
        -spacer_strength_, 20);
}

GraphPhysics& GraphPhysics::instance() {
    if (instance_ == NULL) {
        instance_ = new GraphPhysics();
        instance_->init();
    }

    return *instance_;
}

void GraphPhysics::cleanup() {
    delete instance_;
}


}
