#ifndef LAYER_H
#define LAYER_H

#include <string>
#include <vector>
#include <functional>

namespace Ember {
	static int un_named_counter = 1;
	
	class Layer {
	public:
		Layer(const std::string& name) : layer_name(name) { }
		Layer() : layer_name("layer" + std::to_string(un_named_counter)) { un_named_counter++; }
		std::string Name() { return layer_name; }
	private:
		std::string layer_name;
	};

	class EventStack {
	public:
		enum class EventStackError {
			LayerNameCouldNotBeFound, LayerDeletionFailed, DuplicitLayerFound, Null
		};

		EventStack(const std::string& first_name);
		EventStack() : current_layer(0) { }

		EventStackError SetCurrentLayer(const std::string& name);
		EventStackError AddLayer(const std::string& name);
		EventStackError DeleteLayer(const std::string& name);
		EventStackError DrawEventToLayer(const std::function<void()>& event_func, const std::string& name);

		std::string GetLayer() { return event_layers[current_layer].Name(); }
	private:
		size_t IndexLayers(const std::string& name);

		size_t current_layer;
		std::vector<Layer> event_layers;
	};
}

#endif // !LAYER_H
