/*
  ==============================================================================

    ProcessorGroup.h
    Created: 5 Jun 2024 7:47:10pm
    Author:  cjia

  ==============================================================================
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "ProcessorBase.h"

using Node = juce::AudioProcessorGraph::Node;

class ProcessorNode
{

public:

    juce::String ProcessorName;
    Node::Ptr ProcessorNodePtr;
    bool ProcessorStete;

    ProcessorNode()
    {
        ProcessorStete = false;
    }

    ~ProcessorNode()
    {

    }
    void setName(juce::String name)
    {
        ProcessorName = name;
    }
private:

};

template <typename... Layers>
class ProcessorGroup : public juce::AudioProcessorGraph
{

public:
    ProcessorGroup(juce::AudioProcessorValueTreeState *apvts)
    {
        Apvts = apvts;
    }

    ~ProcessorGroup()
    {

    }
    void AudioGroupInit()
    {
        clear(); 

        audioInputNode = addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioInputNode));
        audioOutputNode = addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioOutputNode));
        midiInputNode = addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiInputNode));
        midiOutputNode = addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiOutputNode));

        InputNode->ProcessorName=juce::String("InputNode");
        InputNode->ProcessorNodePtr = audioInputNode;
        InputNode->ProcessorStete = true;

        OutputNode->setName(juce::String("OutputNode"));
        OutputNode->ProcessorNodePtr = audioOutputNode;
        OutputNode->ProcessorStete = true;

        Node.push_back(InputNode);
        Node.push_back(OutputNode);

        updateConnectGroup();

    }
    Node::Ptr addProcessorNode(std::unique_ptr<ProcessorBase> newProcessor, NodeID nodeId = {}, UpdateKind = UpdateKind::sync)
    {
        auto processorNode = std::make_shared<ProcessorNode>();

        newProcessor->init(Apvts);
        processorNode->setName(newProcessor->getName());
        processorNode->ProcessorNodePtr = addNode(std::move(newProcessor));;
        processorNode->ProcessorStete = true;
        Node.insert(Node.end() - 1, processorNode);

        updateConnectGroup();
        return processorNode->ProcessorNodePtr;
    }
    void updateConnectGroup()
    {
        std::shared_ptr<ProcessorNode>  previousNode = nullptr;

        for (auto connection : getConnections()) 
                removeConnection(connection);

        for (auto currentNode : Node)
        {
            if (previousNode!= nullptr)
            {
                for (int channel = 0; channel < 2; ++channel)// stereo in ,channel < 2
                {
                    juce::Logger::outputDebugString("previousNode=" + juce::String(previousNode->ProcessorName));
                    juce::Logger::outputDebugString("currentNode=" + juce::String(currentNode->ProcessorName));
                    auto previousNodeID = previousNode->ProcessorNodePtr->nodeID;
                    auto currentNodeID = currentNode->ProcessorNodePtr->nodeID;
                    addConnection({ {previousNodeID,  channel },
                                    { currentNodeID, channel } });
                }
            }
            previousNode = currentNode;
        }

        addConnection({ { midiInputNode->nodeID,  juce::AudioProcessorGraph::midiChannelIndex },
                           { midiOutputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex } });


   }
    

private:
    std::tuple<Layers...> layers;
    static constexpr size_t n_layers = sizeof...(Layers);

    juce::AudioProcessorValueTreeState *Apvts;

    Node::Ptr audioInputNode;
    Node::Ptr audioOutputNode;
    Node::Ptr midiInputNode;
    Node::Ptr midiOutputNode;
    std::shared_ptr<ProcessorNode> InputNode= std::make_shared<ProcessorNode>();
    std::shared_ptr<ProcessorNode> OutputNode = std::make_shared<ProcessorNode>();
    std::vector<std::shared_ptr<ProcessorNode>> Node;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessorGroup)
};