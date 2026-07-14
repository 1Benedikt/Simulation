#ifndef __SDNPROJECT_PATHENUMERATOR_H
#define __SDNPROJECT_PATHENUMERATOR_H

#include <map>
#include <string>
#include <vector>

namespace sdnproject {

class PathEnumerator
{
  public:
    using Topology = std::map<std::string, std::vector<std::string>>;

    explicit PathEnumerator(const Topology& topology) : topology(topology) {}

    // Returns all equal-cost paths from src to dst.
    // Assumes fat-tree structure: src -> edge -> agg -> core -> agg -> edge -> dst.
    std::vector<std::vector<std::string>> enumerate(const std::string& src, const std::string& dst) const
    {
        std::string srcEdge = firstNeighborWithPrefix(src, "edge");
        std::string dstEdge = firstNeighborWithPrefix(dst, "edge");
        if (srcEdge.empty() || dstEdge.empty())
            return {};

        auto srcAggs = neighborsWithPrefix(srcEdge, "aggregation");
        auto dstAggs = neighborsWithPrefix(dstEdge, "aggregation");

        std::vector<std::vector<std::string>> paths;
        for (const auto& srcAgg : srcAggs) {
            for (const auto& core : neighborsWithPrefix(srcAgg, "core")) {
                for (const auto& dstAgg : dstAggs) {
                    paths.push_back({src, srcEdge, srcAgg, core, dstAgg, dstEdge, dst});
                }
            }
        }
        return paths;
    }

  private:
    const Topology& topology;


    //Get only the first Neighbor for a node
    //Used to get the edge for host
    std::string firstNeighborWithPrefix(const std::string& node, const std::string& prefix) const
    {
        auto it = topology.find(node);
        if (it == topology.end())
            return {};
        for (const auto& neighbor : it->second)
            if (neighbor.rfind(prefix, 0) == 0)
                return neighbor;
        return {};
    }

    //Get all neighbors for a node.
    //Used e.g to get the cores for an aggregator
    std::vector<std::string> neighborsWithPrefix(const std::string& node, const std::string& prefix) const
    {
        std::vector<std::string> result;
        auto it = topology.find(node);
        if (it == topology.end())
            return result;
        for (const auto& neighbor : it->second)
            if (neighbor.rfind(prefix, 0) == 0)
                result.push_back(neighbor);
        return result;
    }
};

} // namespace sdnproject

#endif
