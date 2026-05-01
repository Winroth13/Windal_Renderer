#pragma once
#include <memory>
#include <DirectXMath.h>
#include <DirectXCollision.h>

constexpr size_t MAX_ELEMENTS_IN_NODE = 4;

template<typename T>
class QuadTree
{
public:
	void PrintTree() // Starts the treversal
	{
		PrintTree(root);
	}

	void AddElement(
		const T* elementAddress,
		const DirectX::BoundingBox& boundingBox
	)
	{
		AddToNode(elementAddress, boundingBox, root);
	}

private:
	struct Object
	{
		T element;
		DirectX::BoundingBox bounds;
	};

	struct Node
	{
		std::vector<Object> objects;
		DirectX::BoundingBox bounds;
		std::unique_ptr<Node> children[4] = { nullptr };

		bool IsFull() { return objects.size() >= MAX_ELEMENTS_IN_NODE; }
	};

	std::unique_ptr<Node> root;

	void PrintTree(std::unique_ptr<Node> nodeToProcess)
	{
		// Local information

		if (true) // If leaf?
		{
			// Print leaf info
		}
		else // Is parent
		{
			// Print for each child
		}
	}

	void AddToNode(
		const T* elementAddress,
		const DirectX::BoundingBox& bounds,
		Node* node
	)
	{
		bool collision = node->bounds.Intersects(bounds); // Check for bounding volume collision
		if (!collision) // If no collision, then discard the node
		{
			return;
		}

		if (node->children[0] == nullptr) // Check if collided node is a leaf node
		{
			if (!node->IsFull()) // Node has space
			{
				// Add object and its bounding volume node
				Object object = {};
				object.element = *elementAddress;
				object.bounds = bounds;
				node->elements.emplace_back(object);
				return;
			}
			else { // The node is a leaf node, but it is not empty
				// Expand the current node by adding children nodes
				// Attempt to concurrently add the already esisting objects and
				// the new object into the new children
				for (int i = 0; i < 4; ++i)
				{
					node->children[i] = std::make_unique<Node>();

					DirectX::BoundingBox childBoundingBox = {};

					int x = i % 2;
					int z = i / 2;

					float childBoundingSizeX = node->bounds.Extends.x / 2;
					float childBoundingSizeZ = node->bounds.Extends.z / 2;
					childBoundingBox.Extents = { childBoundingSizeX, node->bounds.Extents.y, childBoundingSizeZ };

					childBoundingBox.Center = node->bounds.Center;
					childBoundingBox.Center.x -= childBoundingSizeX;
					childBoundingBox.Center.z -= childBoundingSizeZ;

					childBoundingBox.Center.x += x * 2 * childBoundingSizeX;
					childBoundingBox.Center.z += z * 2 * childBoundingSizeZ;

					// Old objects
					for (Object& object : node->objects)
					{
						AddToNode(&object.element, object.bounds, node->children[i]);
					}
					// New object
					AddToNode(elementAddress, bounds, node->children[i]);
				}

				node->children.clear();
			}
		}
		else { // Current node is parent node
			// For each children node, recursively call this function with the same
			// object and bounding volume that were recieved by this function call
			for (Node& node : node->children)
			{
				AddToNode(elementAddress, bounds, node);
			}
		}
	}
};